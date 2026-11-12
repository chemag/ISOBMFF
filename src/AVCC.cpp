/*******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 DigiDNA - www.digidna.net
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/*!
 * @file        AVCC.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <AVCC.hpp>
#include <Parser.hpp>
#include <Utils.hpp>

namespace ISOBMFF {
class AVCC::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  uint8_t _configurationVersion;
  uint8_t _AVCProfileIndication;
  uint8_t _profileCompatibility;
  uint8_t _AVCLevelIndication;
  uint8_t _lengthSizeMinusOne;
  uint8_t _numOfSequenceParameterSets;
  std::vector<std::shared_ptr<NALUnit> > _sequence_parameter_set_nal_units;
  uint8_t _numOfPictureParameterSets;
  std::vector<std::shared_ptr<NALUnit> > _picture_parameter_set_nal_units;
  bool _hasHighProfileFields;
  uint8_t _chromaFormat;
  uint8_t _bitDepthLumaMinus8;
  uint8_t _bitDepthChromaMinus8;
  uint8_t _numOfSequenceParameterSetExt;
  std::vector<std::shared_ptr<NALUnit> > _sequence_parameter_set_ext_nal_units;
};

AVCC::AVCC() : Box("avcC"), impl(std::make_unique<IMPL>()) {}

AVCC::AVCC(const AVCC& o) : Box(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

AVCC::AVCC(AVCC&& o) noexcept : Box(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

AVCC::~AVCC() {}

AVCC& AVCC::operator=(AVCC o) {
  Box::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(AVCC& o1, AVCC& o2) {
  using std::swap;

  swap(static_cast<Box&>(o1), static_cast<Box&>(o2));
  swap(o1.impl, o2.impl);
}

Error AVCC::ReadData(Parser& parser, BinaryStream& stream) {
  uint8_t u8;
  uint8_t count;
  uint8_t i;
  Error err;

  (void)parser;

  err = stream.ReadUInt8(u8);
  if (err) return err;
  this->SetConfigurationVersion(u8);

  err = stream.ReadUInt8(u8);
  if (err) return err;
  this->SetAVCProfileIndication(u8);

  err = stream.ReadUInt8(u8);
  if (err) return err;
  this->SetProfileCompatibility(u8);

  err = stream.ReadUInt8(u8);
  if (err) return err;
  this->SetAVCLevelIndication(u8);

  err = stream.ReadUInt8(u8);
  if (err) return err;

  this->SetLengthSizeMinusOne(u8 & 0x3);

  err = stream.ReadUInt8(u8);
  if (err) return err;

  this->SetNumOfSequenceParameterSets(u8 & 0x1f);
  count = this->GetNumOfSequenceParameterSets();
  for (i = 0; i < count; i++) {
    if (stream.HasBytesAvailable() == false) {
      /*
       * Shouldn't happen in theory, but happens on some files...
       * So simply don't process next NAL unit if we don't have any data
       * available...
       */
      break;
    }

    // stop at the first NAL unit that consumes no data (a truncated one)
    size_t start = stream.Tell();
    auto nal_unit = std::make_shared<NALUnit>(stream);
    if (stream.Tell() == start) {
      break;
    }
    this->AddSequenceParameterSetNALUnit(nal_unit);
  }

  err = stream.ReadUInt8(u8);
  if (err) return err;
  this->SetNumOfPictureParameterSets(u8);

  count = this->GetNumOfPictureParameterSets();
  for (i = 0; i < count; i++) {
    if (stream.HasBytesAvailable() == false) {
      /*
       * Shouldn't happen in theory, but happens on some files...
       * So simply don't process next NAL unit if we don't have any data
       * available...
       */
      break;
    }

    // stop at the first NAL unit that consumes no data (a truncated one)
    size_t start = stream.Tell();
    auto nal_unit = std::make_shared<NALUnit>(stream);
    if (stream.Tell() == start) {
      break;
    }
    this->AddPictureParameterSetNALUnit(nal_unit);
  }

  // ISO/IEC 14496-15:2022, section 5.3.2.1.2:
  //   if( AVCProfileIndication != 66 && AVCProfileIndication != 77 &&
  //       AVCProfileIndication != 88 )
  //   {
  //     bit(6) reserved = '111111'b;
  //     unsigned int(2) chroma_format;
  //     bit(5) reserved = '11111'b;
  //     unsigned int(3) bit_depth_luma_minus8;
  //     bit(5) reserved = '11111'b;
  //     unsigned int(3) bit_depth_chroma_minus8;
  //     unsigned int(8) numOfSequenceParameterSetExt;
  //     for (i=0; i< numOfSequenceParameterSetExt; i++) {
  //       unsigned int(16) sequenceParameterSetExtLength;
  //       bit(8*sequenceParameterSetExtLength) sequenceParameterSetExtNALUnit;
  //     }
  //   }
  // Some muxers write High profile avcC boxes that end after the PPSs, so
  // these fields are optional: read them only when all 4 fixed bytes are
  // there, and treat a box that ends earlier as not having them.
  const uint8_t profile = this->GetAVCProfileIndication();
  if (profile == 66 || profile == 77 || profile == 88) {
    return Error();
  }
  uint8_t fields[4];
  for (uint8_t& field : fields) {
    if (stream.ReadUInt8(field)) {
      return Error();
    }
  }
  this->SetHasHighProfileFields(true);
  this->SetChromaFormat(fields[0] & 0x03);
  this->SetBitDepthLumaMinus8(fields[1] & 0x07);
  this->SetBitDepthChromaMinus8(fields[2] & 0x07);
  this->SetNumOfSequenceParameterSetExt(fields[3]);

  count = this->GetNumOfSequenceParameterSetExt();
  for (i = 0; i < count; i++) {
    if (stream.HasBytesAvailable() == false) {
      break;
    }
    // stop at the first NAL unit that consumes no data (a truncated one)
    size_t start = stream.Tell();
    auto nal_unit = std::make_shared<NALUnit>(stream);
    if (stream.Tell() == start) {
      break;
    }
    this->AddSequenceParameterSetExtNALUnit(nal_unit);
  }

  return Error();
}

void AVCC::WriteDescription(std::ostream& os, std::size_t indentLevel) const {
  Box::WriteDescription(os, indentLevel);
  DisplayableObjectContainer::WriteDescription(os, indentLevel);
}

std::vector<std::shared_ptr<DisplayableObject> > AVCC::GetDisplayableObjects()
    const {
  auto sps(this->GetSequenceParameterSetNALUnits());
  auto vsps =
      std::vector<std::shared_ptr<DisplayableObject> >(sps.begin(), sps.end());
  auto pps(this->GetPictureParameterSetNALUnits());
  auto vpps =
      std::vector<std::shared_ptr<DisplayableObject> >(pps.begin(), pps.end());
  vsps.insert(vsps.end(), vpps.begin(), vpps.end());
  auto spse(this->GetSequenceParameterSetExtNALUnits());
  vsps.insert(vsps.end(), spse.begin(), spse.end());
  return vsps;
}

std::vector<std::pair<std::string, std::string> >
AVCC::GetDisplayableProperties() const {
  auto props(Box::GetDisplayableProperties());

  props.push_back({"Configuration version",
                   std::to_string(this->GetConfigurationVersion())});
  props.push_back(
      {"AVC profile", std::to_string(this->GetAVCProfileIndication())});
  props.push_back({"Profile compatibility",
                   std::to_string(this->GetProfileCompatibility())});
  props.push_back({"AVC level", std::to_string(this->GetAVCLevelIndication())});
  props.push_back({"Length size (minus one)",
                   std::to_string(this->GetLengthSizeMinusOne())});
  props.push_back({"Number of Sequence Parameter Sets",
                   std::to_string(this->GetNumOfSequenceParameterSets())});
  props.push_back({"Number of Picture Parameter Sets",
                   std::to_string(this->GetNumOfPictureParameterSets())});
  props.push_back(
      {"SPS NAL Units",
       std::to_string(this->GetSequenceParameterSetNALUnits().size())});
  props.push_back(
      {"PPS NAL Units",
       std::to_string(this->GetPictureParameterSetNALUnits().size())});
  if (this->HasHighProfileFields()) {
    props.push_back({"Chroma format", std::to_string(this->GetChromaFormat())});
    props.push_back({"Bit depth luma minus 8",
                     std::to_string(this->GetBitDepthLumaMinus8())});
    props.push_back({"Bit depth chroma minus 8",
                     std::to_string(this->GetBitDepthChromaMinus8())});
    props.push_back({"Number of Sequence Parameter Set Extensions",
                     std::to_string(this->GetNumOfSequenceParameterSetExt())});
    props.push_back(
        {"SPS Extension NAL Units",
         std::to_string(this->GetSequenceParameterSetExtNALUnits().size())});
  }

  return props;
}

uint8_t AVCC::GetConfigurationVersion() const {
  return this->impl->_configurationVersion;
}

uint8_t AVCC::GetAVCProfileIndication() const {
  return this->impl->_AVCProfileIndication;
}

uint8_t AVCC::GetProfileCompatibility() const {
  return this->impl->_profileCompatibility;
}

uint8_t AVCC::GetAVCLevelIndication() const {
  return this->impl->_AVCLevelIndication;
}

uint8_t AVCC::GetLengthSizeMinusOne() const {
  return this->impl->_lengthSizeMinusOne;
}

uint8_t AVCC::GetNumOfSequenceParameterSets() const {
  return this->impl->_numOfSequenceParameterSets;
}

uint8_t AVCC::GetNumOfPictureParameterSets() const {
  return this->impl->_numOfPictureParameterSets;
}

void AVCC::SetConfigurationVersion(uint8_t value) {
  this->impl->_configurationVersion = value;
}

void AVCC::SetAVCProfileIndication(uint8_t value) {
  this->impl->_AVCProfileIndication = value;
}

void AVCC::SetProfileCompatibility(uint8_t value) {
  this->impl->_profileCompatibility = value;
}

void AVCC::SetAVCLevelIndication(uint8_t value) {
  this->impl->_AVCLevelIndication = value;
}

void AVCC::SetLengthSizeMinusOne(uint8_t value) {
  this->impl->_lengthSizeMinusOne = value;
}

void AVCC::SetNumOfSequenceParameterSets(uint8_t value) {
  this->impl->_numOfSequenceParameterSets = value;
}

void AVCC::SetNumOfPictureParameterSets(uint8_t value) {
  this->impl->_numOfPictureParameterSets = value;
}

bool AVCC::HasHighProfileFields() const {
  return this->impl->_hasHighProfileFields;
}

uint8_t AVCC::GetChromaFormat() const { return this->impl->_chromaFormat; }

uint8_t AVCC::GetBitDepthLumaMinus8() const {
  return this->impl->_bitDepthLumaMinus8;
}

uint8_t AVCC::GetBitDepthChromaMinus8() const {
  return this->impl->_bitDepthChromaMinus8;
}

uint8_t AVCC::GetNumOfSequenceParameterSetExt() const {
  return this->impl->_numOfSequenceParameterSetExt;
}

void AVCC::SetHasHighProfileFields(bool value) {
  this->impl->_hasHighProfileFields = value;
}

void AVCC::SetChromaFormat(uint8_t value) { this->impl->_chromaFormat = value; }

void AVCC::SetBitDepthLumaMinus8(uint8_t value) {
  this->impl->_bitDepthLumaMinus8 = value;
}

void AVCC::SetBitDepthChromaMinus8(uint8_t value) {
  this->impl->_bitDepthChromaMinus8 = value;
}

void AVCC::SetNumOfSequenceParameterSetExt(uint8_t value) {
  this->impl->_numOfSequenceParameterSetExt = value;
}

std::vector<std::shared_ptr<AVCC::NALUnit> >
AVCC::GetSequenceParameterSetNALUnits() const {
  return this->impl->_sequence_parameter_set_nal_units;
}

std::vector<std::shared_ptr<AVCC::NALUnit> >
AVCC::GetPictureParameterSetNALUnits() const {
  return this->impl->_picture_parameter_set_nal_units;
}

void AVCC::AddSequenceParameterSetNALUnit(std::shared_ptr<NALUnit> nal_unit) {
  this->impl->_sequence_parameter_set_nal_units.push_back(nal_unit);
}

void AVCC::AddPictureParameterSetNALUnit(std::shared_ptr<NALUnit> nal_unit) {
  this->impl->_picture_parameter_set_nal_units.push_back(nal_unit);
}

std::vector<std::shared_ptr<AVCC::NALUnit> >
AVCC::GetSequenceParameterSetExtNALUnits() const {
  return this->impl->_sequence_parameter_set_ext_nal_units;
}

void AVCC::AddSequenceParameterSetExtNALUnit(
    std::shared_ptr<NALUnit> nal_unit) {
  this->impl->_sequence_parameter_set_ext_nal_units.push_back(nal_unit);
}

AVCC::IMPL::IMPL()
    : _configurationVersion(0),
      _AVCProfileIndication(0),
      _profileCompatibility(0),
      _AVCLevelIndication(0),
      _lengthSizeMinusOne(0),
      _numOfSequenceParameterSets(0),
      _numOfPictureParameterSets(0),
      _hasHighProfileFields(false),
      _chromaFormat(0),
      _bitDepthLumaMinus8(0),
      _bitDepthChromaMinus8(0),
      _numOfSequenceParameterSetExt(0) {}

AVCC::IMPL::IMPL(const IMPL& o)
    : _configurationVersion(o._configurationVersion),
      _AVCProfileIndication(o._AVCProfileIndication),
      _profileCompatibility(o._profileCompatibility),
      _AVCLevelIndication(o._AVCLevelIndication),
      _lengthSizeMinusOne(o._lengthSizeMinusOne),
      _numOfSequenceParameterSets(o._numOfSequenceParameterSets),
      _sequence_parameter_set_nal_units(o._sequence_parameter_set_nal_units),
      _numOfPictureParameterSets(o._numOfPictureParameterSets),
      _picture_parameter_set_nal_units(o._picture_parameter_set_nal_units),
      _hasHighProfileFields(o._hasHighProfileFields),
      _chromaFormat(o._chromaFormat),
      _bitDepthLumaMinus8(o._bitDepthLumaMinus8),
      _bitDepthChromaMinus8(o._bitDepthChromaMinus8),
      _numOfSequenceParameterSetExt(o._numOfSequenceParameterSetExt),
      _sequence_parameter_set_ext_nal_units(
          o._sequence_parameter_set_ext_nal_units) {}

AVCC::IMPL::~IMPL() {}
}  // namespace ISOBMFF

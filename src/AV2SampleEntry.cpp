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
 * @file        AV2SampleEntry.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <AV2SampleEntry.hpp>
#include <ContainerBox.hpp>
#include <FixedPoint.hpp>

namespace ISOBMFF {
class AV2SampleEntry::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  uint16_t _data_reference_index;
  uint16_t _width;
  uint16_t _height;
  uint32_t _horizresolution;
  uint32_t _vertresolution;
  uint16_t _frame_count;
  std::string _compressorname;
  uint16_t _depth;

  std::vector<std::shared_ptr<Box> > _boxes;
};

AV2SampleEntry::AV2SampleEntry(const std::string& name)
    : FullBox(name), impl(std::make_unique<IMPL>()) {}

AV2SampleEntry::AV2SampleEntry(const AV2SampleEntry& o)
    : FullBox(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

AV2SampleEntry::AV2SampleEntry(AV2SampleEntry&& o) noexcept
    : FullBox(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

AV2SampleEntry::~AV2SampleEntry() {}

AV2SampleEntry& AV2SampleEntry::operator=(AV2SampleEntry o) {
  FullBox::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(AV2SampleEntry& o1, AV2SampleEntry& o2) {
  using std::swap;

  swap(static_cast<FullBox&>(o1), static_cast<FullBox&>(o2));
  swap(o1.impl, o2.impl);
}

Error AV2SampleEntry::ReadData(Parser& parser, BinaryStream& stream) {
  ContainerBox container("????");
  Error err;

  uint8_t temp8;
  uint16_t temp16;
  uint32_t temp32;

  err = stream.ReadUInt8(temp8);
  if (err) return err;
  err = stream.ReadUInt8(temp8);
  if (err) return err;
  err = stream.ReadUInt8(temp8);
  if (err) return err;
  err = stream.ReadUInt8(temp8);
  if (err) return err;
  err = stream.ReadUInt8(temp8);
  if (err) return err;
  err = stream.ReadUInt8(temp8);
  if (err) return err;

  err = stream.ReadBigEndianUInt16(temp16);
  if (err) return err;
  this->SetDataReferenceIndex(temp16);

  err = stream.ReadBigEndianUInt16(temp16);
  if (err) return err;
  err = stream.ReadBigEndianUInt16(temp16);
  if (err) return err;
  err = stream.ReadBigEndianUInt32(temp32);
  if (err) return err;
  err = stream.ReadBigEndianUInt32(temp32);
  if (err) return err;
  err = stream.ReadBigEndianUInt32(temp32);
  if (err) return err;

  err = stream.ReadBigEndianUInt16(temp16);
  if (err) return err;
  this->SetWidth(temp16);

  err = stream.ReadBigEndianUInt16(temp16);
  if (err) return err;
  this->SetHeight(temp16);

  err = stream.ReadBigEndianUInt32(temp32);
  if (err) return err;
  this->SetHorizResolutionRaw(temp32);

  err = stream.ReadBigEndianUInt32(temp32);
  if (err) return err;
  this->SetVertResolutionRaw(temp32);

  err = stream.ReadBigEndianUInt32(temp32);
  if (err) return err;

  err = stream.ReadBigEndianUInt16(temp16);
  if (err) return err;
  this->SetFrameCount(temp16);

  std::string tempStr;
  err = stream.ReadString(tempStr, 32);
  if (err) return err;
  this->SetCompressorName(tempStr);

  err = stream.ReadBigEndianUInt16(temp16);
  if (err) return err;
  this->SetDepth(temp16);

  err = stream.ReadBigEndianUInt16(temp16);
  if (err) return err;

  err = container.ReadData(parser, stream);
  if (err) return err;

  this->impl->_boxes = container.GetBoxes();

  return Error();
}

std::vector<std::pair<std::string, std::string> >
AV2SampleEntry::GetDisplayableProperties() const {
  auto props(FullBox::GetDisplayableProperties());

  props.push_back(
      {"Data Reference Index", std::to_string(this->GetDataReferenceIndex())});
  props.push_back({"Width", std::to_string(this->GetWidth())});
  props.push_back({"Height", std::to_string(this->GetHeight())});
  props.push_back({"Horizontal Resolution (raw)",
                   std::to_string(this->GetHorizResolutionRaw())});
  props.push_back(
      {"Horizontal Resolution", std::to_string(this->GetHorizResolution())});
  props.push_back({"Vertical Resolution (raw)",
                   std::to_string(this->GetVertResolutionRaw())});
  props.push_back(
      {"Vertical Resolution", std::to_string(this->GetVertResolution())});
  props.push_back({"Frame Count", std::to_string(this->GetFrameCount())});
  props.push_back({"Compressor Name", this->GetCompressorName()});
  props.push_back({"Depth", std::to_string(this->GetDepth())});

  return props;
}

void AV2SampleEntry::WriteDescription(std::ostream& os,
                                      std::size_t indentLevel) const {
  FullBox::WriteDescription(os, indentLevel);
  Container::WriteBoxes(os, indentLevel);
}

uint16_t AV2SampleEntry::GetDataReferenceIndex() const {
  return this->impl->_data_reference_index;
}

uint16_t AV2SampleEntry::GetWidth() const { return this->impl->_width; }

uint16_t AV2SampleEntry::GetHeight() const { return this->impl->_height; }

uint32_t AV2SampleEntry::GetHorizResolutionRaw() const {
  return this->impl->_horizresolution;
}

uint32_t AV2SampleEntry::GetVertResolutionRaw() const {
  return this->impl->_vertresolution;
}

uint16_t AV2SampleEntry::GetFrameCount() const {
  return this->impl->_frame_count;
}

std::string AV2SampleEntry::GetCompressorName() const {
  return this->impl->_compressorname;
}

uint16_t AV2SampleEntry::GetDepth() const { return this->impl->_depth; }

void AV2SampleEntry::SetDataReferenceIndex(uint16_t value) {
  this->impl->_data_reference_index = value;
}

void AV2SampleEntry::SetWidth(uint16_t value) { this->impl->_width = value; }

void AV2SampleEntry::SetHeight(uint16_t value) { this->impl->_height = value; }

void AV2SampleEntry::SetHorizResolutionRaw(uint32_t value) {
  this->impl->_horizresolution = value;
}

void AV2SampleEntry::SetVertResolutionRaw(uint32_t value) {
  this->impl->_vertresolution = value;
}

void AV2SampleEntry::SetFrameCount(uint16_t value) {
  this->impl->_frame_count = value;
}

void AV2SampleEntry::SetCompressorName(std::string value) {
  this->impl->_compressorname = value;
}

void AV2SampleEntry::SetDepth(uint16_t value) { this->impl->_depth = value; }

void AV2SampleEntry::AddBox(std::shared_ptr<Box> box) {
  if (box != nullptr) {
    this->impl->_boxes.push_back(box);
  }
}

std::vector<std::shared_ptr<Box> > AV2SampleEntry::GetBoxes() const {
  return this->impl->_boxes;
}

double AV2SampleEntry::GetHorizResolution() const {
  return FixedPoint::ToDouble(this->impl->_horizresolution, 16);
}

double AV2SampleEntry::GetVertResolution() const {
  return FixedPoint::ToDouble(this->impl->_vertresolution, 16);
}

void AV2SampleEntry::SetHorizResolution(double value) {
  this->impl->_horizresolution = FixedPoint::FromDouble<uint32_t>(value, 16);
}

void AV2SampleEntry::SetVertResolution(double value) {
  this->impl->_vertresolution = FixedPoint::FromDouble<uint32_t>(value, 16);
}

AV2SampleEntry::IMPL::IMPL()
    : _data_reference_index(0),
      _width(0),
      _height(0),
      _horizresolution(0),
      _vertresolution(0),
      _frame_count(0),
      _compressorname(""),
      _depth(0) {}

AV2SampleEntry::IMPL::IMPL(const IMPL& o)
    : _data_reference_index(o._data_reference_index),
      _width(o._width),
      _height(o._height),
      _horizresolution(o._horizresolution),
      _vertresolution(o._vertresolution),
      _frame_count(o._frame_count),
      _compressorname(o._compressorname),
      _depth(o._depth),
      _boxes(o._boxes) {}

AV2SampleEntry::IMPL::~IMPL() {}
}  // namespace ISOBMFF

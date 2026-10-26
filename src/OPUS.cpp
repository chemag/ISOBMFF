/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @file        OPUS.cpp
 */

#include <ContainerBox.hpp>
#include <OPUS.hpp>

namespace ISOBMFF {
class OPUS::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  uint16_t _datareferenceindex;
  uint16_t _channelcount;
  uint16_t _samplesize;
  uint32_t _samplerate;
  std::vector<std::shared_ptr<Box> > _boxes;
};

OPUS::OPUS() : FullBox("Opus"), impl(std::make_unique<IMPL>()) {}

OPUS::OPUS(const OPUS& o)
    : FullBox(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

OPUS::OPUS(OPUS&& o) noexcept : FullBox(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

OPUS::~OPUS() {}

OPUS& OPUS::operator=(OPUS o) {
  FullBox::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(OPUS& o1, OPUS& o2) {
  using std::swap;

  swap(static_cast<FullBox&>(o1), static_cast<FullBox&>(o2));
  swap(o1.impl, o2.impl);
}

Error OPUS::ReadData(Parser& parser, BinaryStream& stream) {
  Error err;
  ContainerBox container("????");

  // const unsigned int(8)[6] reserved = 0;
  uint8_t temp8;
  for (int i = 0; i < 6; i++) {
    err = stream.ReadUInt8(temp8);
    if (err) return err;
  }
  // unsigned int(16) data_reference_index;
  uint16_t dataReferenceIndex;
  err = stream.ReadBigEndianUInt16(dataReferenceIndex);
  if (err) return err;
  SetDataReferenceIndex(dataReferenceIndex);
  // const unsigned int(32)[2] reserved = 0;
  uint32_t temp32;
  err = stream.ReadUInt32(temp32);
  if (err) return err;
  err = stream.ReadUInt32(temp32);
  if (err) return err;

  // unsigned int(16) channelcount;
  uint16_t channelCount;
  err = stream.ReadBigEndianUInt16(channelCount);
  if (err) return err;
  SetChannelCount(channelCount);

  // template unsigned int(16) samplesize = 16;
  uint16_t sampleSize;
  err = stream.ReadBigEndianUInt16(sampleSize);
  if (err) return err;
  SetSampleSize(sampleSize);

  // unsigned int(16) pre_defined = 0;
  uint16_t temp16;
  err = stream.ReadUInt16(temp16);
  if (err) return err;
  // const unsigned int(16) reserved = 0;
  err = stream.ReadUInt16(temp16);
  if (err) return err;

  // template unsigned int(32) samplerate = 48000 << 16;
  uint32_t sampleRate;
  err = stream.ReadBigEndianUInt32(sampleRate);
  if (err) return err;
  SetSampleRateRaw(sampleRate);

  // OpusSpecificBox (dOps), plus any optional boxes (e.g. btrt)
  err = container.ReadData(parser, stream);
  if (err) return err;
  this->impl->_boxes = container.GetBoxes();
  return Error();
}

std::vector<std::pair<std::string, std::string> >
OPUS::GetDisplayableProperties() const {
  auto props(FullBox::GetDisplayableProperties());

  props.push_back(
      {"Data Reference Index", std::to_string(this->GetDataReferenceIndex())});
  props.push_back({"Channel Count", std::to_string(this->GetChannelCount())});
  props.push_back({"Sample Size", std::to_string(this->GetSampleSize())});
  props.push_back(
      {"Sample Rate (raw)", std::to_string(this->GetSampleRateRaw())});
  props.push_back({"Sample Rate", std::to_string(this->GetSampleRate())});

  return props;
}

void OPUS::WriteDescription(std::ostream& os, std::size_t indentLevel) const {
  FullBox::WriteDescription(os, indentLevel);
  Container::WriteBoxes(os, indentLevel);
}

uint16_t OPUS::GetDataReferenceIndex() const {
  return this->impl->_datareferenceindex;
}

uint16_t OPUS::GetChannelCount() const { return this->impl->_channelcount; }

uint16_t OPUS::GetSampleSize() const { return this->impl->_samplesize; }

uint32_t OPUS::GetSampleRateRaw() const { return this->impl->_samplerate; }

float OPUS::GetSampleRate() const {
  uint32_t sampleRateFixed = this->GetSampleRateRaw();
  // The sample rate is stored as a 16.16 fixed-point number (32 bits).
  float sampleRate = (sampleRateFixed >> 16);
  sampleRate += ((sampleRateFixed & 0xffff) / 65536.0f);
  return sampleRate;
}

void OPUS::SetDataReferenceIndex(uint16_t datareferenceindex) {
  this->impl->_datareferenceindex = datareferenceindex;
}

void OPUS::SetChannelCount(uint16_t channelcount) {
  this->impl->_channelcount = channelcount;
}

void OPUS::SetSampleSize(uint16_t samplesize) {
  this->impl->_samplesize = samplesize;
}

void OPUS::SetSampleRateRaw(uint32_t samplerate) {
  this->impl->_samplerate = samplerate;
}

void OPUS::AddBox(std::shared_ptr<Box> box) {
  if (box != nullptr) {
    this->impl->_boxes.push_back(box);
  }
}

std::vector<std::shared_ptr<Box> > OPUS::GetBoxes() const {
  return this->impl->_boxes;
}

OPUS::IMPL::IMPL()
    : _datareferenceindex(0),
      _channelcount(0),
      _samplesize(0),
      _samplerate(0) {}

OPUS::IMPL::IMPL(const IMPL& o)
    : _datareferenceindex(o._datareferenceindex),
      _channelcount(o._channelcount),
      _samplesize(o._samplesize),
      _samplerate(o._samplerate),
      _boxes(o._boxes) {}

OPUS::IMPL::~IMPL() {}
}  // namespace ISOBMFF

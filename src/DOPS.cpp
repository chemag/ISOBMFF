/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @file        DOPS.cpp
 */

#include <DOPS.hpp>
#include <Parser.hpp>

namespace ISOBMFF {
class DOPS::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  uint8_t _version;
  uint8_t _outputchannelcount;
  uint16_t _preskip;
  uint32_t _inputsamplerate;
  int16_t _outputgain;
  uint8_t _channelmappingfamily;
  uint8_t _streamcount;
  uint8_t _coupledcount;
  std::vector<uint8_t> _channelmapping;
};

DOPS::DOPS() : Box("dOps"), impl(std::make_unique<IMPL>()) {}

DOPS::DOPS(const DOPS& o) : Box(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

DOPS::DOPS(DOPS&& o) noexcept : Box(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

DOPS::~DOPS() {}

DOPS& DOPS::operator=(DOPS o) {
  Box::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(DOPS& o1, DOPS& o2) {
  using std::swap;

  swap(static_cast<Box&>(o1), static_cast<Box&>(o2));
  swap(o1.impl, o2.impl);
}

Error DOPS::ReadData(Parser& parser, BinaryStream& stream) {
  Error err;
  (void)parser;

  // unsigned int(8) Version;
  uint8_t version;
  err = stream.ReadUInt8(version);
  if (err) return err;
  SetVersion(version);

  // unsigned int(8) OutputChannelCount;
  uint8_t outputChannelCount;
  err = stream.ReadUInt8(outputChannelCount);
  if (err) return err;
  SetOutputChannelCount(outputChannelCount);

  // unsigned int(16) PreSkip;
  uint16_t preSkip;
  err = stream.ReadBigEndianUInt16(preSkip);
  if (err) return err;
  SetPreSkip(preSkip);

  // unsigned int(32) InputSampleRate;
  uint32_t inputSampleRate;
  err = stream.ReadBigEndianUInt32(inputSampleRate);
  if (err) return err;
  SetInputSampleRate(inputSampleRate);

  // signed int(16) OutputGain;
  uint16_t outputGain;
  err = stream.ReadBigEndianUInt16(outputGain);
  if (err) return err;
  SetOutputGain(static_cast<int16_t>(outputGain));

  // unsigned int(8) ChannelMappingFamily;
  uint8_t channelMappingFamily;
  err = stream.ReadUInt8(channelMappingFamily);
  if (err) return err;
  SetChannelMappingFamily(channelMappingFamily);

  if (channelMappingFamily != 0) {
    // ChannelMappingTable
    // unsigned int(8) StreamCount;
    uint8_t streamCount;
    err = stream.ReadUInt8(streamCount);
    if (err) return err;
    SetStreamCount(streamCount);

    // unsigned int(8) CoupledCount;
    uint8_t coupledCount;
    err = stream.ReadUInt8(coupledCount);
    if (err) return err;
    SetCoupledCount(coupledCount);

    // unsigned int(8 * OutputChannelCount) ChannelMapping;
    std::vector<uint8_t> channelMapping;
    for (uint8_t i = 0; i < outputChannelCount; i++) {
      uint8_t mapping;
      err = stream.ReadUInt8(mapping);
      if (err) return err;
      channelMapping.push_back(mapping);
    }
    SetChannelMapping(channelMapping);
  }

  return Error();
}

std::vector<std::pair<std::string, std::string> >
DOPS::GetDisplayableProperties() const {
  auto props(Box::GetDisplayableProperties());

  props.push_back({"Version", std::to_string(this->GetVersion())});
  props.push_back(
      {"Output Channel Count", std::to_string(this->GetOutputChannelCount())});
  props.push_back({"Pre Skip", std::to_string(this->GetPreSkip())});
  props.push_back(
      {"Input Sample Rate", std::to_string(this->GetInputSampleRate())});
  props.push_back({"Output Gain", std::to_string(this->GetOutputGain())});
  props.push_back({"Channel Mapping Family",
                   std::to_string(this->GetChannelMappingFamily())});
  if (this->GetChannelMappingFamily() != 0) {
    props.push_back({"Stream Count", std::to_string(this->GetStreamCount())});
    props.push_back({"Coupled Count", std::to_string(this->GetCoupledCount())});
    std::string channelMapping;
    for (uint8_t mapping : this->GetChannelMapping()) {
      if (!channelMapping.empty()) {
        channelMapping += ", ";
      }
      channelMapping += std::to_string(mapping);
    }
    props.push_back({"Channel Mapping", channelMapping});
  }

  return props;
}

uint8_t DOPS::GetVersion() const { return this->impl->_version; }

uint8_t DOPS::GetOutputChannelCount() const {
  return this->impl->_outputchannelcount;
}

uint16_t DOPS::GetPreSkip() const { return this->impl->_preskip; }

uint32_t DOPS::GetInputSampleRate() const {
  return this->impl->_inputsamplerate;
}

int16_t DOPS::GetOutputGain() const { return this->impl->_outputgain; }

uint8_t DOPS::GetChannelMappingFamily() const {
  return this->impl->_channelmappingfamily;
}

uint8_t DOPS::GetStreamCount() const { return this->impl->_streamcount; }

uint8_t DOPS::GetCoupledCount() const { return this->impl->_coupledcount; }

std::vector<uint8_t> DOPS::GetChannelMapping() const {
  return this->impl->_channelmapping;
}

void DOPS::SetVersion(uint8_t value) { this->impl->_version = value; }

void DOPS::SetOutputChannelCount(uint8_t value) {
  this->impl->_outputchannelcount = value;
}

void DOPS::SetPreSkip(uint16_t value) { this->impl->_preskip = value; }

void DOPS::SetInputSampleRate(uint32_t value) {
  this->impl->_inputsamplerate = value;
}

void DOPS::SetOutputGain(int16_t value) { this->impl->_outputgain = value; }

void DOPS::SetChannelMappingFamily(uint8_t value) {
  this->impl->_channelmappingfamily = value;
}

void DOPS::SetStreamCount(uint8_t value) { this->impl->_streamcount = value; }

void DOPS::SetCoupledCount(uint8_t value) { this->impl->_coupledcount = value; }

void DOPS::SetChannelMapping(const std::vector<uint8_t>& value) {
  this->impl->_channelmapping = value;
}

DOPS::IMPL::IMPL()
    : _version(0),
      _outputchannelcount(0),
      _preskip(0),
      _inputsamplerate(0),
      _outputgain(0),
      _channelmappingfamily(0),
      _streamcount(0),
      _coupledcount(0) {}

DOPS::IMPL::IMPL(const IMPL& o)
    : _version(o._version),
      _outputchannelcount(o._outputchannelcount),
      _preskip(o._preskip),
      _inputsamplerate(o._inputsamplerate),
      _outputgain(o._outputgain),
      _channelmappingfamily(o._channelmappingfamily),
      _streamcount(o._streamcount),
      _coupledcount(o._coupledcount),
      _channelmapping(o._channelmapping) {}

DOPS::IMPL::~IMPL() {}
}  // namespace ISOBMFF

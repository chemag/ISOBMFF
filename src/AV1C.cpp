/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @file        AV1C.cpp
 */

#include <AV1C.hpp>
#include <Parser.hpp>
#include <iomanip>
#include <sstream>

namespace ISOBMFF {
class AV1C::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  uint8_t _marker;
  uint8_t _version;
  uint8_t _seq_profile;
  uint8_t _seq_level_idx_0;
  uint8_t _seq_tier_0;
  uint8_t _high_bitdepth;
  uint8_t _twelve_bit;
  uint8_t _monochrome;
  uint8_t _chroma_subsampling_x;
  uint8_t _chroma_subsampling_y;
  uint8_t _chroma_sample_position;
  uint8_t _initial_presentation_delay_present;
  uint8_t _initial_presentation_delay_minus_one;
  std::vector<uint8_t> _config_obus;
};

AV1C::AV1C() : Box("av1C"), impl(std::make_unique<IMPL>()) {}

AV1C::AV1C(const AV1C& o) : Box(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

AV1C::AV1C(AV1C&& o) noexcept : Box(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

AV1C::~AV1C() {}

AV1C& AV1C::operator=(AV1C o) {
  Box::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(AV1C& o1, AV1C& o2) {
  using std::swap;

  swap(static_cast<Box&>(o1), static_cast<Box&>(o2));
  swap(o1.impl, o2.impl);
}

Error AV1C::ReadData(Parser& parser, BinaryStream& stream) {
  Error err;
  uint8_t u8;
  (void)parser;

  // unsigned int(1) marker = 1;
  // unsigned int(7) version = 1;
  err = stream.ReadUInt8(u8);
  if (err) return err;
  this->SetMarker((u8 >> 7) & 0x01);
  this->SetVersion(u8 & 0x7f);

  // unsigned int(3) seq_profile;
  // unsigned int(5) seq_level_idx_0;
  err = stream.ReadUInt8(u8);
  if (err) return err;
  this->SetSeqProfile((u8 >> 5) & 0x07);
  this->SetSeqLevelIdx0(u8 & 0x1f);

  // unsigned int(1) seq_tier_0;
  // unsigned int(1) high_bitdepth;
  // unsigned int(1) twelve_bit;
  // unsigned int(1) monochrome;
  // unsigned int(1) chroma_subsampling_x;
  // unsigned int(1) chroma_subsampling_y;
  // unsigned int(2) chroma_sample_position;
  err = stream.ReadUInt8(u8);
  if (err) return err;
  this->SetSeqTier0((u8 >> 7) & 0x01);
  this->SetHighBitdepth((u8 >> 6) & 0x01);
  this->SetTwelveBit((u8 >> 5) & 0x01);
  this->SetMonochrome((u8 >> 4) & 0x01);
  this->SetChromaSubsamplingX((u8 >> 3) & 0x01);
  this->SetChromaSubsamplingY((u8 >> 2) & 0x01);
  this->SetChromaSamplePosition(u8 & 0x03);

  // unsigned int(3) reserved = 0;
  // unsigned int(1) initial_presentation_delay_present;
  // if (initial_presentation_delay_present) {
  //   unsigned int(4) initial_presentation_delay_minus_one;
  // } else {
  //   unsigned int(4) reserved = 0;
  // }
  err = stream.ReadUInt8(u8);
  if (err) return err;
  this->SetInitialPresentationDelayPresent((u8 >> 4) & 0x01);
  this->SetInitialPresentationDelayMinusOne(
      this->GetInitialPresentationDelayPresent() ? (u8 & 0x0f) : 0);

  // unsigned int(8) configOBUs[];
  std::vector<uint8_t> config_obus;
  err = stream.ReadAllData(config_obus);
  if (err) return err;
  this->SetConfigOBUs(config_obus);

  return Error();
}

std::vector<std::pair<std::string, std::string> >
AV1C::GetDisplayableProperties() const {
  auto props(Box::GetDisplayableProperties());

  props.push_back({"Marker", std::to_string(this->GetMarker())});
  props.push_back({"Version", std::to_string(this->GetVersion())});
  props.push_back({"Seq Profile", std::to_string(this->GetSeqProfile())});
  props.push_back({"Seq Level Idx 0", std::to_string(this->GetSeqLevelIdx0())});
  props.push_back({"Seq Tier 0", std::to_string(this->GetSeqTier0())});
  props.push_back({"High Bitdepth", std::to_string(this->GetHighBitdepth())});
  props.push_back({"Twelve Bit", std::to_string(this->GetTwelveBit())});
  props.push_back({"Monochrome", std::to_string(this->GetMonochrome())});
  props.push_back(
      {"Chroma Subsampling X", std::to_string(this->GetChromaSubsamplingX())});
  props.push_back(
      {"Chroma Subsampling Y", std::to_string(this->GetChromaSubsamplingY())});
  props.push_back({"Chroma Sample Position",
                   std::to_string(this->GetChromaSamplePosition())});
  props.push_back({"Initial Presentation Delay Present",
                   std::to_string(this->GetInitialPresentationDelayPresent())});
  if (this->GetInitialPresentationDelayPresent()) {
    props.push_back(
        {"Initial Presentation Delay Minus One",
         std::to_string(this->GetInitialPresentationDelayMinusOne())});
  }
  std::vector<uint8_t> config_obus = this->GetConfigOBUs();
  std::stringstream ss;
  for (size_t i = 0; i < config_obus.size(); i++) {
    ss << (i ? " " : "") << std::hex << std::uppercase << std::setfill('0')
       << std::setw(2) << static_cast<uint32_t>(config_obus[i]);
  }
  props.push_back({"Config OBUs Length", std::to_string(config_obus.size())});
  props.push_back({"Config OBUs", ss.str()});

  return props;
}

uint8_t AV1C::GetMarker() const { return this->impl->_marker; }

uint8_t AV1C::GetVersion() const { return this->impl->_version; }

uint8_t AV1C::GetSeqProfile() const { return this->impl->_seq_profile; }

uint8_t AV1C::GetSeqLevelIdx0() const { return this->impl->_seq_level_idx_0; }

uint8_t AV1C::GetSeqTier0() const { return this->impl->_seq_tier_0; }

uint8_t AV1C::GetHighBitdepth() const { return this->impl->_high_bitdepth; }

uint8_t AV1C::GetTwelveBit() const { return this->impl->_twelve_bit; }

uint8_t AV1C::GetMonochrome() const { return this->impl->_monochrome; }

uint8_t AV1C::GetChromaSubsamplingX() const {
  return this->impl->_chroma_subsampling_x;
}

uint8_t AV1C::GetChromaSubsamplingY() const {
  return this->impl->_chroma_subsampling_y;
}

uint8_t AV1C::GetChromaSamplePosition() const {
  return this->impl->_chroma_sample_position;
}

uint8_t AV1C::GetInitialPresentationDelayPresent() const {
  return this->impl->_initial_presentation_delay_present;
}

uint8_t AV1C::GetInitialPresentationDelayMinusOne() const {
  return this->impl->_initial_presentation_delay_minus_one;
}

std::vector<uint8_t> AV1C::GetConfigOBUs() const {
  return this->impl->_config_obus;
}

void AV1C::SetMarker(uint8_t value) { this->impl->_marker = value; }

void AV1C::SetVersion(uint8_t value) { this->impl->_version = value; }

void AV1C::SetSeqProfile(uint8_t value) { this->impl->_seq_profile = value; }

void AV1C::SetSeqLevelIdx0(uint8_t value) {
  this->impl->_seq_level_idx_0 = value;
}

void AV1C::SetSeqTier0(uint8_t value) { this->impl->_seq_tier_0 = value; }

void AV1C::SetHighBitdepth(uint8_t value) {
  this->impl->_high_bitdepth = value;
}

void AV1C::SetTwelveBit(uint8_t value) { this->impl->_twelve_bit = value; }

void AV1C::SetMonochrome(uint8_t value) { this->impl->_monochrome = value; }

void AV1C::SetChromaSubsamplingX(uint8_t value) {
  this->impl->_chroma_subsampling_x = value;
}

void AV1C::SetChromaSubsamplingY(uint8_t value) {
  this->impl->_chroma_subsampling_y = value;
}

void AV1C::SetChromaSamplePosition(uint8_t value) {
  this->impl->_chroma_sample_position = value;
}

void AV1C::SetInitialPresentationDelayPresent(uint8_t value) {
  this->impl->_initial_presentation_delay_present = value;
}

void AV1C::SetInitialPresentationDelayMinusOne(uint8_t value) {
  this->impl->_initial_presentation_delay_minus_one = value;
}

void AV1C::SetConfigOBUs(const std::vector<uint8_t>& value) {
  this->impl->_config_obus = value;
}

AV1C::IMPL::IMPL()
    : _marker(0),
      _version(0),
      _seq_profile(0),
      _seq_level_idx_0(0),
      _seq_tier_0(0),
      _high_bitdepth(0),
      _twelve_bit(0),
      _monochrome(0),
      _chroma_subsampling_x(0),
      _chroma_subsampling_y(0),
      _chroma_sample_position(0),
      _initial_presentation_delay_present(0),
      _initial_presentation_delay_minus_one(0) {}

AV1C::IMPL::IMPL(const IMPL& o)
    : _marker(o._marker),
      _version(o._version),
      _seq_profile(o._seq_profile),
      _seq_level_idx_0(o._seq_level_idx_0),
      _seq_tier_0(o._seq_tier_0),
      _high_bitdepth(o._high_bitdepth),
      _twelve_bit(o._twelve_bit),
      _monochrome(o._monochrome),
      _chroma_subsampling_x(o._chroma_subsampling_x),
      _chroma_subsampling_y(o._chroma_subsampling_y),
      _chroma_sample_position(o._chroma_sample_position),
      _initial_presentation_delay_present(
          o._initial_presentation_delay_present),
      _initial_presentation_delay_minus_one(
          o._initial_presentation_delay_minus_one),
      _config_obus(o._config_obus) {}

AV1C::IMPL::~IMPL() {}
}  // namespace ISOBMFF

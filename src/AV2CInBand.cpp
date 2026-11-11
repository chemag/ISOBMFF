/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @file        AV2CInBand.cpp
 */

#include <AV2CInBand.hpp>
#include <Parser.hpp>

namespace ISOBMFF {
class AV2CInBand::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  uint8_t _reserved1;
  uint8_t _max_num_streams_minus1;
  uint8_t _max_profile_idc;
  uint8_t _max_level_idx;
  uint8_t _max_tier;
};

AV2CInBand::AV2CInBand() : Box("av2c"), impl(std::make_unique<IMPL>()) {}

AV2CInBand::AV2CInBand(const AV2CInBand& o)
    : Box(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

AV2CInBand::AV2CInBand(AV2CInBand&& o) noexcept
    : Box(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

AV2CInBand::~AV2CInBand() {}

AV2CInBand& AV2CInBand::operator=(AV2CInBand o) {
  Box::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(AV2CInBand& o1, AV2CInBand& o2) {
  using std::swap;

  swap(static_cast<Box&>(o1), static_cast<Box&>(o2));
  swap(o1.impl, o2.impl);
}

Error AV2CInBand::ReadData(Parser& parser, BinaryStream& stream) {
  Error err;
  uint16_t u16;
  (void)parser;

  // unsigned int(3) reserved1 = 0;
  // unsigned int(2) max_num_streams_minus1;
  // unsigned int(5) max_profile_idc;
  // unsigned int(5) max_level_idx;
  // unsigned int(1) max_tier;
  err = stream.ReadBigEndianUInt16(u16);
  if (err) return err;
  this->SetReserved1(static_cast<uint8_t>((u16 >> 13) & 0x07));
  this->SetMaxNumStreamsMinus1(static_cast<uint8_t>((u16 >> 11) & 0x03));
  this->SetMaxProfileIdc(static_cast<uint8_t>((u16 >> 6) & 0x1f));
  this->SetMaxLevelIdx(static_cast<uint8_t>((u16 >> 1) & 0x1f));
  this->SetMaxTier(static_cast<uint8_t>(u16 & 0x01));

  return Error();
}

std::vector<std::pair<std::string, std::string> >
AV2CInBand::GetDisplayableProperties() const {
  auto props(Box::GetDisplayableProperties());

  props.push_back({"Reserved1", std::to_string(this->GetReserved1())});
  props.push_back({"Max Num Streams Minus1",
                   std::to_string(this->GetMaxNumStreamsMinus1())});
  props.push_back(
      {"Max Profile Idc", std::to_string(this->GetMaxProfileIdc())});
  props.push_back({"Max Level Idx", std::to_string(this->GetMaxLevelIdx())});
  props.push_back({"Max Tier", std::to_string(this->GetMaxTier())});

  return props;
}

uint8_t AV2CInBand::GetReserved1() const { return this->impl->_reserved1; }

uint8_t AV2CInBand::GetMaxNumStreamsMinus1() const {
  return this->impl->_max_num_streams_minus1;
}

uint8_t AV2CInBand::GetMaxProfileIdc() const {
  return this->impl->_max_profile_idc;
}

uint8_t AV2CInBand::GetMaxLevelIdx() const {
  return this->impl->_max_level_idx;
}

uint8_t AV2CInBand::GetMaxTier() const { return this->impl->_max_tier; }

void AV2CInBand::SetReserved1(uint8_t value) { this->impl->_reserved1 = value; }

void AV2CInBand::SetMaxNumStreamsMinus1(uint8_t value) {
  this->impl->_max_num_streams_minus1 = value;
}

void AV2CInBand::SetMaxProfileIdc(uint8_t value) {
  this->impl->_max_profile_idc = value;
}

void AV2CInBand::SetMaxLevelIdx(uint8_t value) {
  this->impl->_max_level_idx = value;
}

void AV2CInBand::SetMaxTier(uint8_t value) { this->impl->_max_tier = value; }

AV2CInBand::IMPL::IMPL()
    : _reserved1(0),
      _max_num_streams_minus1(0),
      _max_profile_idc(0),
      _max_level_idx(0),
      _max_tier(0) {}

AV2CInBand::IMPL::IMPL(const IMPL& o)
    : _reserved1(o._reserved1),
      _max_num_streams_minus1(o._max_num_streams_minus1),
      _max_profile_idc(o._max_profile_idc),
      _max_level_idx(o._max_level_idx),
      _max_tier(o._max_tier) {}

AV2CInBand::IMPL::~IMPL() {}
}  // namespace ISOBMFF

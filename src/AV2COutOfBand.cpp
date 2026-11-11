/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @file        AV2COutOfBand.cpp
 */

#include <AV2COutOfBand.hpp>
#include <Parser.hpp>
#include <iomanip>
#include <sstream>

namespace ISOBMFF {
class AV2COutOfBand::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  uint8_t _reserved1;
  uint8_t _config_obus_count_minus1;
  std::vector<std::vector<uint8_t> > _config_obus;
};

AV2COutOfBand::AV2COutOfBand() : Box("av2C"), impl(std::make_unique<IMPL>()) {}

AV2COutOfBand::AV2COutOfBand(const AV2COutOfBand& o)
    : Box(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

AV2COutOfBand::AV2COutOfBand(AV2COutOfBand&& o) noexcept
    : Box(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

AV2COutOfBand::~AV2COutOfBand() {}

AV2COutOfBand& AV2COutOfBand::operator=(AV2COutOfBand o) {
  Box::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(AV2COutOfBand& o1, AV2COutOfBand& o2) {
  using std::swap;

  swap(static_cast<Box&>(o1), static_cast<Box&>(o2));
  swap(o1.impl, o2.impl);
}

namespace {
// leb128(), as defined in the AV2 specification: at most 8 bytes, little
// endian groups of 7 bits, with the top bit of each byte set if another byte
// follows. The value must fit in 32 bits.
Error ReadLeb128(BinaryStream& stream, uint32_t& value) {
  uint64_t v = 0;
  for (unsigned i = 0; i < 8; i++) {
    uint8_t byte;
    Error err = stream.ReadUInt8(byte);
    if (err) return err;
    v |= static_cast<uint64_t>(byte & 0x7f) << (i * 7);
    if ((byte & 0x80) == 0) {
      if (v > 0xffffffff) {
        return Error(ErrorCode::InvalidBoxData, "leb128 value too large");
      }
      value = static_cast<uint32_t>(v);
      return Error();
    }
  }
  return Error(ErrorCode::InvalidBoxData, "leb128 value longer than 8 bytes");
}
}  // namespace

Error AV2COutOfBand::ReadData(Parser& parser, BinaryStream& stream) {
  Error err;
  uint8_t u8;
  (void)parser;

  // unsigned int(8) reserved1 = 0;
  err = stream.ReadUInt8(u8);
  if (err) return err;
  this->SetReserved1(u8);

  // unsigned int(8) config_obus_count_minus1;
  err = stream.ReadUInt8(u8);
  if (err) return err;
  this->SetConfigOBUsCountMinus1(u8);

  // for (i = 0; i < config_obus_count_minus1 + 1; ++i) {
  //   unsigned int(8) config_obu[];
  // }
  // where each config_obu is a leb128() length followed by that many bytes
  std::vector<std::vector<uint8_t> > config_obus;
  for (unsigned i = 0; i < static_cast<unsigned>(u8) + 1; i++) {
    uint32_t obu_length;
    err = ReadLeb128(stream, obu_length);
    if (err) return err;
    std::vector<uint8_t> obu;
    err = stream.Read(obu, obu_length);
    if (err) return err;
    config_obus.push_back(obu);
  }
  this->SetConfigOBUs(config_obus);

  return Error();
}

std::vector<std::pair<std::string, std::string> >
AV2COutOfBand::GetDisplayableProperties() const {
  auto props(Box::GetDisplayableProperties());

  props.push_back({"Reserved1", std::to_string(this->GetReserved1())});
  props.push_back({"Config OBUs Count Minus1",
                   std::to_string(this->GetConfigOBUsCountMinus1())});
  std::vector<std::vector<uint8_t> > config_obus = this->GetConfigOBUs();
  for (size_t i = 0; i < config_obus.size(); i++) {
    std::stringstream ss;
    for (size_t j = 0; j < config_obus[i].size(); j++) {
      ss << (j ? " " : "") << std::hex << std::uppercase << std::setfill('0')
         << std::setw(2) << static_cast<uint32_t>(config_obus[i][j]);
    }
    props.push_back({"Config OBU " + std::to_string(i) + " Length",
                     std::to_string(config_obus[i].size())});
    props.push_back({"Config OBU " + std::to_string(i), ss.str()});
  }

  return props;
}

uint8_t AV2COutOfBand::GetReserved1() const { return this->impl->_reserved1; }

uint8_t AV2COutOfBand::GetConfigOBUsCountMinus1() const {
  return this->impl->_config_obus_count_minus1;
}

std::vector<std::vector<uint8_t> > AV2COutOfBand::GetConfigOBUs() const {
  return this->impl->_config_obus;
}

void AV2COutOfBand::SetReserved1(uint8_t value) {
  this->impl->_reserved1 = value;
}

void AV2COutOfBand::SetConfigOBUsCountMinus1(uint8_t value) {
  this->impl->_config_obus_count_minus1 = value;
}

void AV2COutOfBand::SetConfigOBUs(
    const std::vector<std::vector<uint8_t> >& value) {
  this->impl->_config_obus = value;
}

AV2COutOfBand::IMPL::IMPL() : _reserved1(0), _config_obus_count_minus1(0) {}

AV2COutOfBand::IMPL::IMPL(const IMPL& o)
    : _reserved1(o._reserved1),
      _config_obus_count_minus1(o._config_obus_count_minus1),
      _config_obus(o._config_obus) {}

AV2COutOfBand::IMPL::~IMPL() {}
}  // namespace ISOBMFF

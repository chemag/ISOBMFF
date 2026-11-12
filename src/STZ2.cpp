/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @file        STZ2.cpp
 */

#include <Parser.hpp>
#include <STZ2.hpp>
#include <cstdint>

namespace ISOBMFF {
class STZ2::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  uint8_t _field_size;
  uint32_t _sample_count;
  std::vector<uint16_t> _entry_size;
};

STZ2::STZ2() : FullBox("stz2"), impl(std::make_unique<IMPL>()) {}

STZ2::STZ2(const STZ2& o)
    : FullBox(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

STZ2::STZ2(STZ2&& o) noexcept : FullBox(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

STZ2::~STZ2() {}

STZ2& STZ2::operator=(STZ2 o) {
  FullBox::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(STZ2& o1, STZ2& o2) {
  using std::swap;

  swap(static_cast<FullBox&>(o1), static_cast<FullBox&>(o2));
  swap(o1.impl, o2.impl);
}

Error STZ2::ReadData(Parser& parser, BinaryStream& stream) {
  Error err;

  err = FullBox::ReadData(parser, stream);
  if (err) return err;

  // unsigned int(24) reserved = 0;
  uint8_t reserved[3];
  for (uint8_t& byte : reserved) {
    err = stream.ReadUInt8(byte);
    if (err) return err;
  }

  // unsigned int(8) field_size;
  err = stream.ReadUInt8(this->impl->_field_size);
  if (err) return err;
  if (this->impl->_field_size != 4 && this->impl->_field_size != 8 &&
      this->impl->_field_size != 16) {
    return Error(ErrorCode::InvalidBoxData,
                 "stz2: invalid field_size " +
                     std::to_string(this->impl->_field_size) +
                     " (must be 4, 8, or 16)");
  }

  // unsigned int(32) sample_count;
  err = stream.ReadBigEndianUInt32(this->impl->_sample_count);
  if (err) return err;

  // for (i = 1; i <= sample_count; i++) {
  //   unsigned int(field_size) entry_size;
  // }
  //
  // With field_size 4, each byte holds two entries, the first one in the
  // upper nibble; an odd sample_count leaves the last lower nibble unused.
  // Read the entries one at a time, rather than reserving sample_count of
  // them up front: sample_count comes from the file, and a box that holds
  // fewer entries than it declares fails at the first missing one.
  uint32_t i = 0;
  while (i < this->impl->_sample_count) {
    if (this->impl->_field_size == 16) {
      uint16_t entry_size;
      err = stream.ReadBigEndianUInt16(entry_size);
      if (err) {
        break;
      }
      this->impl->_entry_size.push_back(entry_size);
      i++;
    } else {
      uint8_t byte;
      err = stream.ReadUInt8(byte);
      if (err) {
        break;
      }
      if (this->impl->_field_size == 8) {
        this->impl->_entry_size.push_back(byte);
        i++;
      } else {
        this->impl->_entry_size.push_back(static_cast<uint16_t>(byte >> 4));
        i++;
        if (i < this->impl->_sample_count) {
          this->impl->_entry_size.push_back(static_cast<uint16_t>(byte & 0x0f));
          i++;
        }
      }
    }
  }
  if (err) {
    return Error(ErrorCode::InsufficientData,
                 "stz2: sample_count is " +
                     std::to_string(this->impl->_sample_count) +
                     " but the box holds only " + std::to_string(i) +
                     " entry_size values");
  }
  return Error();
}

std::vector<std::pair<std::string, std::string> >
STZ2::GetDisplayableProperties() const {
  auto props(FullBox::GetDisplayableProperties());

  props.push_back({"Field Size", std::to_string(this->GetFieldSize())});
  props.push_back({"Sample Count", std::to_string(this->GetSampleCount())});
  for (size_t index = 0; index < this->GetEntryCount(); index++) {
    props.push_back({"Entry Size", std::to_string(this->GetEntrySize(index))});
  }

  return props;
}

uint8_t STZ2::GetFieldSize() const { return this->impl->_field_size; }

uint32_t STZ2::GetSampleCount() const { return this->impl->_sample_count; }

size_t STZ2::GetEntryCount() const { return this->impl->_entry_size.size(); }

uint16_t STZ2::GetEntrySize(size_t index) const {
  return this->impl->_entry_size[index];
}

STZ2::IMPL::IMPL() : _field_size(0), _sample_count(0) {}

STZ2::IMPL::IMPL(const IMPL& o)
    : _field_size(o._field_size),
      _sample_count(o._sample_count),
      _entry_size(o._entry_size) {}

STZ2::IMPL::~IMPL() {}
}  // namespace ISOBMFF

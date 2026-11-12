/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @file        STSZ.cpp
 */

#include <Parser.hpp>
#include <STSZ.hpp>
#include <cstdint>

namespace ISOBMFF {
class STSZ::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  uint32_t _sample_size;
  uint32_t _sample_count;
  std::vector<uint32_t> _entry_size;
};

STSZ::STSZ() : FullBox("stsz"), impl(std::make_unique<IMPL>()) {}

STSZ::STSZ(const STSZ& o)
    : FullBox(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

STSZ::STSZ(STSZ&& o) noexcept : FullBox(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

STSZ::~STSZ() {}

STSZ& STSZ::operator=(STSZ o) {
  FullBox::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(STSZ& o1, STSZ& o2) {
  using std::swap;

  swap(static_cast<FullBox&>(o1), static_cast<FullBox&>(o2));
  swap(o1.impl, o2.impl);
}

Error STSZ::ReadData(Parser& parser, BinaryStream& stream) {
  Error err;

  err = FullBox::ReadData(parser, stream);
  if (err) return err;

  // unsigned int(32) sample_size;
  err = stream.ReadBigEndianUInt32(this->impl->_sample_size);
  if (err) return err;

  // unsigned int(32) sample_count;
  err = stream.ReadBigEndianUInt32(this->impl->_sample_count);
  if (err) return err;

  if (this->impl->_sample_size != 0) {
    // all the samples have the same size: there is no table
    return Error();
  }

  // if (sample_size == 0) {
  //   for (i = 1; i <= sample_count; i++) {
  //     unsigned int(32) entry_size;
  //   }
  // }
  //
  // Read the entries one at a time, rather than reserving sample_count of
  // them up front: sample_count comes from the file, and a box that holds
  // fewer entries than it declares fails at the first missing one.
  for (uint32_t i = 0; i < this->impl->_sample_count; i++) {
    uint32_t entry_size;
    err = stream.ReadBigEndianUInt32(entry_size);
    if (err) {
      return Error(ErrorCode::InsufficientData,
                   "stsz: sample_count is " +
                       std::to_string(this->impl->_sample_count) +
                       " but the box holds only " + std::to_string(i) +
                       " entry_size values");
    }
    this->impl->_entry_size.push_back(entry_size);
  }
  return Error();
}

std::vector<std::pair<std::string, std::string> >
STSZ::GetDisplayableProperties() const {
  auto props(FullBox::GetDisplayableProperties());

  props.push_back({"Sample Size", std::to_string(this->GetSampleSize())});
  props.push_back({"Sample Count", std::to_string(this->GetSampleCount())});
  for (size_t index = 0; index < this->GetEntryCount(); index++) {
    props.push_back({"Entry Size", std::to_string(this->GetEntrySize(index))});
  }

  return props;
}

uint32_t STSZ::GetSampleSize() const { return this->impl->_sample_size; }

uint32_t STSZ::GetSampleCount() const { return this->impl->_sample_count; }

size_t STSZ::GetEntryCount() const { return this->impl->_entry_size.size(); }

uint32_t STSZ::GetEntrySize(size_t index) const {
  return this->impl->_entry_size[index];
}

STSZ::IMPL::IMPL() : _sample_size(0), _sample_count(0) {}

STSZ::IMPL::IMPL(const IMPL& o)
    : _sample_size(o._sample_size),
      _sample_count(o._sample_count),
      _entry_size(o._entry_size) {}

STSZ::IMPL::~IMPL() {}
}  // namespace ISOBMFF

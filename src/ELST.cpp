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
 * @file        ELST.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <ELST.hpp>
#include <FixedPoint.hpp>
#include <Parser.hpp>
#include <cstdint>
#include <cstring>

namespace ISOBMFF {
class ELST::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  std::vector<uint64_t> _edit_duration;
  std::vector<int64_t> _media_time;
  std::vector<int16_t> _media_rate_integer;
  std::vector<int16_t> _media_rate_fraction;
};

ELST::ELST() : FullBox("elst"), impl(std::make_unique<IMPL>()) {}

ELST::ELST(const ELST& o)
    : FullBox(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

ELST::ELST(ELST&& o) noexcept : FullBox(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

ELST::~ELST() {}

ELST& ELST::operator=(ELST o) {
  FullBox::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(ELST& o1, ELST& o2) {
  using std::swap;

  swap(static_cast<FullBox&>(o1), static_cast<FullBox&>(o2));
  swap(o1.impl, o2.impl);
}

Error ELST::ReadData(Parser& parser, BinaryStream& stream) {
  Error err;

  err = FullBox::ReadData(parser, stream);
  if (err) return err;

  uint32_t entry_count;
  err = stream.ReadBigEndianUInt32(entry_count);
  if (err) return err;

  for (uint32_t i = 0; i < entry_count; i++) {
    if (this->GetVersion() == 1) {
      uint64_t edit_duration;
      err = stream.ReadBigEndianUInt64(edit_duration);
      if (err) return err;
      this->impl->_edit_duration.push_back(edit_duration);

      uint64_t media_time_unsigned;
      err = stream.ReadBigEndianUInt64(media_time_unsigned);
      if (err) return err;
      this->impl->_media_time.push_back(
          static_cast<int64_t>(media_time_unsigned));
    } else {
      uint32_t edit_duration;
      err = stream.ReadBigEndianUInt32(edit_duration);
      if (err) return err;
      this->impl->_edit_duration.push_back(edit_duration);

      uint32_t media_time_unsigned;
      err = stream.ReadBigEndianUInt32(media_time_unsigned);
      if (err) return err;
      this->impl->_media_time.push_back(
          static_cast<int32_t>(media_time_unsigned));
    }

    uint16_t media_rate_integer;
    err = stream.ReadBigEndianUInt16(media_rate_integer);
    if (err) return err;
    this->impl->_media_rate_integer.push_back(
        static_cast<int16_t>(media_rate_integer));

    uint16_t media_rate_fraction;
    err = stream.ReadBigEndianUInt16(media_rate_fraction);
    if (err) return err;
    this->impl->_media_rate_fraction.push_back(
        static_cast<int16_t>(media_rate_fraction));
  }
  return Error();
}

std::vector<std::pair<std::string, std::string> >
ELST::GetDisplayableProperties() const {
  auto props(FullBox::GetDisplayableProperties());

  props.push_back({"Entry count", std::to_string(this->GetEntryCount())});

  for (size_t index = 0; index < this->GetEntryCount(); index++) {
    props.push_back(
        {"Edit Duration", std::to_string(this->GetEditDuration(index))});
    props.push_back({"Media Time", std::to_string(this->GetMediaTime(index))});
    props.push_back({"Media Rate Integer",
                     std::to_string(this->GetMediaRateInteger(index))});
    props.push_back({"Media Rate Fraction",
                     std::to_string(this->GetMediaRateFraction(index))});
    props.push_back({"Media Rate", std::to_string(this->GetMediaRate(index))});
  }

  return props;
}

size_t ELST::GetEntryCount() const { return this->impl->_edit_duration.size(); }

uint64_t ELST::GetEditDuration(size_t index) const {
  return this->impl->_edit_duration[index];
}

int64_t ELST::GetMediaTime(size_t index) const {
  return this->impl->_media_time[index];
}

int16_t ELST::GetMediaRateInteger(size_t index) const {
  return this->impl->_media_rate_integer[index];
}

int16_t ELST::GetMediaRateFraction(size_t index) const {
  return this->impl->_media_rate_fraction[index];
}

double ELST::GetMediaRate(size_t index) const {
  // ISO/IEC 14496-12:2026, section 8.6.6.3: media_rate "is expressed as a
  // 16.16 fixed-point integer (16 bits each for the integer and fractional
  // part)". Put the two 16-bit halves back together as the 32-bit
  // fixed-point value they form, so that negative rates (e.g. -0.5, stored
  // as 0xffff8000) convert correctly.
  const uint32_t raw =
      (static_cast<uint32_t>(
           static_cast<uint16_t>(this->impl->_media_rate_integer[index]))
       << 16) |
      static_cast<uint16_t>(this->impl->_media_rate_fraction[index]);
  return FixedPoint::ToDouble(static_cast<int32_t>(raw), 16);
}

ELST::IMPL::IMPL() {}

ELST::IMPL::IMPL(const IMPL& o) {
  this->_edit_duration = o._edit_duration;
  this->_media_time = o._media_time;
  this->_media_rate_integer = o._media_rate_integer;
  this->_media_rate_fraction = o._media_rate_fraction;
}

ELST::IMPL::~IMPL() {}
}  // namespace ISOBMFF

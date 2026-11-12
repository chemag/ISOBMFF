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
 * @file        URL.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <URL.hpp>
#include <string>
#include <vector>

namespace ISOBMFF {
class URL::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  std::string _location;
};

namespace {
// Reads the utf8string fields left in the box: each one ends at a NUL byte
// or at the end of the box. Some writers leave out the last NUL, so a
// string that runs into the end of the box is kept rather than rejected.
std::vector<std::string> ReadStrings(BinaryStream& stream, size_t count) {
  std::vector<std::string> strings;
  while (strings.size() < count && stream.HasBytesAvailable()) {
    std::string value;
    uint8_t c = 0;
    while (stream.HasBytesAvailable() && !stream.ReadUInt8(c) && c != 0) {
      value.append(1, static_cast<char>(c));
    }
    strings.push_back(value);
  }
  return strings;
}
}  // namespace

URL::URL() : FullBox("url "), impl(std::make_unique<IMPL>()) {}

URL::URL(const URL& o) : FullBox(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

URL::URL(URL&& o) noexcept : FullBox(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

URL::~URL() {}

URL& URL::operator=(URL o) {
  FullBox::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(URL& o1, URL& o2) {
  using std::swap;

  swap(static_cast<FullBox&>(o1), static_cast<FullBox&>(o2));
  swap(o1.impl, o2.impl);
}

Error URL::ReadData(Parser& parser, BinaryStream& stream) {
  Error err = FullBox::ReadData(parser, stream);
  if (err) return err;

  // ISO/IEC 14496-12:2026, section 8.7.2.3: with flag 0x000001 "the media
  // data is in the same file as the Box containing this data reference
  // [...] no string is present; the box terminates with the entry-flags
  // field". Otherwise location "is required in a URL entry". A missing
  // location is left empty.
  std::vector<std::string> strings = ReadStrings(stream, 1);
  this->SetLocation(strings.size() > 0 ? strings[0] : "");

  return Error();
}

std::vector<std::pair<std::string, std::string> >
URL::GetDisplayableProperties() const {
  auto props(FullBox::GetDisplayableProperties());

  props.push_back({"Location", this->GetLocation()});

  return props;
}

std::string URL::GetLocation() const { return this->impl->_location; }

void URL::SetLocation(const std::string& value) {
  this->impl->_location = value;
}

URL::IMPL::IMPL() {}

URL::IMPL::IMPL(const IMPL& o) : _location(o._location) {}

URL::IMPL::~IMPL() {}
}  // namespace ISOBMFF

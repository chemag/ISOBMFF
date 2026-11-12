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
 * @file        ISPE.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <ISPE.hpp>

namespace ISOBMFF {
class ISPE::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  uint32_t _imageWidth;
  uint32_t _imageHeight;
};

ISPE::ISPE() : FullBox("ispe"), impl(std::make_unique<IMPL>()) {}

ISPE::ISPE(const ISPE& o)
    : FullBox(o), impl(std::make_unique<IMPL>(*(o.impl))) {}

ISPE::ISPE(ISPE&& o) noexcept : FullBox(std::move(o)), impl(std::move(o.impl)) {
  o.impl = nullptr;
}

ISPE::~ISPE() {}

ISPE& ISPE::operator=(ISPE o) {
  FullBox::operator=(o);
  swap(*(this), o);

  return *(this);
}

void swap(ISPE& o1, ISPE& o2) {
  using std::swap;

  swap(static_cast<FullBox&>(o1), static_cast<FullBox&>(o2));
  swap(o1.impl, o2.impl);
}

Error ISPE::ReadData(Parser& parser, BinaryStream& stream) {
  Error err;

  err = FullBox::ReadData(parser, stream);
  if (err) return err;

  uint32_t imageWidth;
  err = stream.ReadBigEndianUInt32(imageWidth);
  if (err) return err;
  this->SetImageWidth(imageWidth);

  uint32_t imageHeight;
  err = stream.ReadBigEndianUInt32(imageHeight);
  if (err) return err;
  this->SetImageHeight(imageHeight);
  return Error();
}

std::vector<std::pair<std::string, std::string> >
ISPE::GetDisplayableProperties() const {
  auto props(FullBox::GetDisplayableProperties());

  props.push_back({"Image width", std::to_string(this->GetImageWidth())});
  props.push_back({"Image height", std::to_string(this->GetImageHeight())});

  return props;
}

uint32_t ISPE::GetImageWidth() const { return this->impl->_imageWidth; }

uint32_t ISPE::GetImageHeight() const { return this->impl->_imageHeight; }

void ISPE::SetImageWidth(uint32_t value) { this->impl->_imageWidth = value; }

void ISPE::SetImageHeight(uint32_t value) { this->impl->_imageHeight = value; }

ISPE::IMPL::IMPL() : _imageWidth(0), _imageHeight(0) {}

ISPE::IMPL::IMPL(const IMPL& o)
    : _imageWidth(o._imageWidth), _imageHeight(o._imageHeight) {}

ISPE::IMPL::~IMPL() {}
}  // namespace ISOBMFF

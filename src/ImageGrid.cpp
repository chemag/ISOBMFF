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
 * @file        ImageGrid.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <ImageGrid.hpp>

namespace ISOBMFF {
class ImageGrid::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  uint8_t _version;
  uint8_t _flags;
  uint8_t _rows;
  uint8_t _columns;
  uint64_t _outputWidth;
  uint64_t _outputHeight;
};

ImageGrid::ImageGrid() : impl(std::make_unique<IMPL>()) {}

ImageGrid::ImageGrid(BinaryStream& stream) : impl(std::make_unique<IMPL>()) {
  uint8_t s;
  uint8_t temp8;

  Error err = stream.ReadUInt8(temp8);
  if (!err) this->SetVersion(temp8);

  err = stream.ReadUInt8(temp8);
  if (!err) this->SetFlags(temp8);

  err = stream.ReadUInt8(temp8);
  if (!err) this->SetRows(temp8);

  err = stream.ReadUInt8(temp8);
  if (!err) this->SetColumns(temp8);

  s = ((this->GetFlags() & 1) + 1) * 2;

  if (s == 1) {
    err = stream.ReadUInt8(temp8);
    if (!err) this->SetOutputWidth(temp8);

    err = stream.ReadUInt8(temp8);
    if (!err) this->SetOutputHeight(temp8);
  } else if (s == 2) {
    uint16_t temp16;
    err = stream.ReadBigEndianUInt16(temp16);
    if (!err) this->SetOutputWidth(temp16);

    err = stream.ReadBigEndianUInt16(temp16);
    if (!err) this->SetOutputHeight(temp16);
  } else if (s == 4) {
    uint32_t temp32;
    err = stream.ReadBigEndianUInt32(temp32);
    if (!err) this->SetOutputWidth(temp32);

    err = stream.ReadBigEndianUInt32(temp32);
    if (!err) this->SetOutputHeight(temp32);
  } else if (s == 8) {
    uint64_t temp64;
    err = stream.ReadBigEndianUInt64(temp64);
    if (!err) this->SetOutputWidth(temp64);

    err = stream.ReadBigEndianUInt64(temp64);
    if (!err) this->SetOutputHeight(temp64);
  }
}

ImageGrid::ImageGrid(const ImageGrid& o)
    : impl(std::make_unique<IMPL>(*(o.impl))) {}

ImageGrid::ImageGrid(ImageGrid&& o) noexcept : impl(std::move(o.impl)) {
  o.impl = nullptr;
}

ImageGrid::~ImageGrid() {}

ImageGrid& ImageGrid::operator=(ImageGrid o) {
  swap(*(this), o);

  return *(this);
}

void swap(ImageGrid& o1, ImageGrid& o2) {
  using std::swap;

  swap(o1.impl, o2.impl);
}

std::string ImageGrid::GetName() const { return "ImageGrid"; }

uint8_t ImageGrid::GetVersion() const { return this->impl->_version; }

uint8_t ImageGrid::GetFlags() const { return this->impl->_flags; }

uint8_t ImageGrid::GetRows() const { return this->impl->_rows; }

uint8_t ImageGrid::GetColumns() const { return this->impl->_columns; }

uint64_t ImageGrid::GetOutputWidth() const { return this->impl->_outputWidth; }

uint64_t ImageGrid::GetOutputHeight() const {
  return this->impl->_outputHeight;
}

void ImageGrid::SetVersion(uint8_t value) { this->impl->_version = value; }

void ImageGrid::SetFlags(uint8_t value) { this->impl->_flags = value; }

void ImageGrid::SetRows(uint8_t value) { this->impl->_rows = value; }

void ImageGrid::SetColumns(uint8_t value) { this->impl->_columns = value; }

void ImageGrid::SetOutputWidth(uint64_t value) {
  this->impl->_outputWidth = value;
}

void ImageGrid::SetOutputHeight(uint64_t value) {
  this->impl->_outputHeight = value;
}

std::vector<std::pair<std::string, std::string> >
ImageGrid::GetDisplayableProperties() const {
  return {
      {"Version", std::to_string(this->GetVersion())},
      {"Flags", std::to_string(this->GetFlags())},
      {"Rows", std::to_string(this->GetRows())},
      {"Columns", std::to_string(this->GetColumns())},
      {"Output width", std::to_string(this->GetOutputWidth())},
      {"Output height", std::to_string(this->GetOutputHeight())},
  };
}

ImageGrid::IMPL::IMPL()
    : _version(0),
      _flags(0),
      _rows(0),
      _columns(0),
      _outputWidth(0),
      _outputHeight(0) {}

ImageGrid::IMPL::IMPL(const IMPL& o)
    : _version(o._version),
      _flags(o._flags),
      _rows(o._rows),
      _columns(o._columns),
      _outputWidth(o._outputWidth),
      _outputHeight(o._outputHeight) {}

ImageGrid::IMPL::~IMPL() {}
}  // namespace ISOBMFF

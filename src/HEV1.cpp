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
 * @file        HEV1.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <HEV1.hpp>

namespace ISOBMFF {
HEV1::HEV1() : HVC1() {}

HEV1::HEV1(const HEV1& o) : HVC1(o) {}

HEV1::HEV1(HEV1&& o) noexcept : HVC1(std::move(o)) {}

HEV1::~HEV1() {}

HEV1& HEV1::operator=(HEV1 o) {
  HVC1::operator=(o);
  swap(*(this), o);

  return *(this);
}

std::string HEV1::GetName() const { return "hev1"; }

void swap(HEV1& o1, HEV1& o2) {
  using std::swap;

  swap(static_cast<HVC1&>(o1), static_cast<HVC1&>(o2));
}
}  // namespace ISOBMFF
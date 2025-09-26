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
 * @header      AVC3.hpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#ifndef ISOBMFF_AVC3_HPP
#define ISOBMFF_AVC3_HPP

#include <AVC1.hpp>
#include <Macros.hpp>
#include <memory>

namespace ISOBMFF {
class ISOBMFF_EXPORT AVC3 : public AVC1 {
 public:
  AVC3();
  AVC3(const AVC3& o);
  AVC3(AVC3&& o) noexcept;
  virtual ~AVC3() override;

  AVC3& operator=(AVC3 o);

  std::string GetName() const override;

  ISOBMFF_EXPORT friend void swap(AVC3& o1, AVC3& o2);
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_AVC3_HPP */
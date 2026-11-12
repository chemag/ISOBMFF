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
 * @header      IROT.hpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#ifndef ISOBMFF_IROT_HPP
#define ISOBMFF_IROT_HPP

#include <FullBox.hpp>
#include <Macros.hpp>
#include <algorithm>
#include <memory>

namespace ISOBMFF {
// ImageRotation ("irot", ISO/IEC 23008-12:2022, section 6.5.10): rotates the
// reconstructed image of the associated image item anti-clockwise, in units
// of 90 degrees. The 2-bit angle field holds the number of 90-degree steps:
// "angle * 90 specifies the angle (in anti-clockwise direction) in units of
// degrees."
class ISOBMFF_EXPORT IROT : public Box {
 public:
  IROT();
  IROT(const IROT& o);
  IROT(IROT&& o) noexcept;
  virtual ~IROT() override;

  IROT& operator=(IROT o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;

  // the angle field as stored (0..3)
  uint8_t GetAngleRaw() const;
  void SetAngleRaw(uint8_t value);
  // the anti-clockwise rotation in degrees: 0, 90, 180, or 270. SetAngle()
  // stores (value / 90) % 4, so values that are not multiples of 90 round
  // down to one
  uint16_t GetAngle() const;
  void SetAngle(uint16_t value);

  ISOBMFF_EXPORT friend void swap(IROT& o1, IROT& o2);

 private:
  class IMPL;

  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_IROT_HPP */

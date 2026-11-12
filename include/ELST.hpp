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
 * @header      ELST.hpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#ifndef ISOBMFF_ELST_HPP
#define ISOBMFF_ELST_HPP

#include <FullBox.hpp>
#include <Macros.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>

namespace ISOBMFF {
class ISOBMFF_EXPORT ELST : public FullBox {
 public:
  ELST();
  ELST(const ELST& o);
  ELST(ELST&& o) noexcept;
  virtual ~ELST() override;

  ELST& operator=(ELST o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;

  size_t GetEntryCount() const;
  uint64_t GetEditDuration(size_t index) const;
  int64_t GetMediaTime(size_t index) const;
  // media_rate_integer and media_rate_fraction, as stored: together they
  // are media_rate, a 16.16 fixed-point number (1.0 is normal speed)
  int16_t GetMediaRateInteger(size_t index) const;
  int16_t GetMediaRateFraction(size_t index) const;
  // media_rate as a double: e.g. 1.0 for normal-speed forward play, 0.0 for
  // a dwell
  double GetMediaRate(size_t index) const;

  ISOBMFF_EXPORT friend void swap(ELST& o1, ELST& o2);

 private:
  class IMPL;

  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_ELST_HPP */

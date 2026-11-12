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
 * @header      URN.hpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#ifndef ISOBMFF_URN_HPP
#define ISOBMFF_URN_HPP

#include <FullBox.hpp>
#include <Macros.hpp>
#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace ISOBMFF {
// DataEntryUrnBox ("urn ", ISO/IEC 14496-12:2026, section 8.7.2):
//   utf8string name;      // a URN, required
//   utf8string location;  // a URL where the named resource can be found,
//                         // optional
class ISOBMFF_EXPORT URN : public FullBox {
 public:
  URN();
  URN(const URN& o);
  URN(URN&& o) noexcept;
  virtual ~URN() override;

  URN& operator=(URN o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;

  // the name field. It is not called GetName(), which is the box type
  // ("urn "), inherited from Box.
  std::string GetURNName() const;
  std::string GetLocation() const;

  void SetURNName(const std::string& value);
  void SetLocation(const std::string& value);

  ISOBMFF_EXPORT friend void swap(URN& o1, URN& o2);

 private:
  class IMPL;

  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_URN_HPP */

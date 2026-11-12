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
 * @header      Matrix.hpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#ifndef ISOBMFF_MATRIX_HPP
#define ISOBMFF_MATRIX_HPP

#include <DisplayableObject.hpp>
#include <Macros.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>

namespace ISOBMFF {
/*!
 * @class       Matrix
 * @abstract    Represents a transformation of video images for presentation.
 * @discussion  ISO/IEC 14496-12:2026, section 7.3.2: "All the values in a
 *              matrix are stored as 16.16 fixed-point values, except for u, v
 *              and w, which are stored as 2.30 fixed-point values." The
 *              getters and setters take the values as doubles, which hold
 *              every stored value exactly.
 */
class ISOBMFF_EXPORT Matrix : public DisplayableObject {
 public:
  /*!
   * @function    Matrix
   * @abstract    Default constructor.
   */
  Matrix();

  /*!
   * @function    Matrix
   * @abstract    Creates a Matrix object from the values stored in the file
   *              (template int(32) matrix[9], in the order {a,b,u, c,d,v,
   *              x,y,w}): a, b, c, d, x and y as 16.16 fixed point, and u, v
   *              and w as 2.30 fixed point.
   * @param       a   The matrix A coordinate (16.16).
   * @param       b   The matrix B coordinate (16.16).
   * @param       u   The matrix U coordinate (2.30).
   * @param       c   The matrix C coordinate (16.16).
   * @param       d   The matrix D coordinate (16.16).
   * @param       v   The matrix V coordinate (2.30).
   * @param       x   The matrix X coordinate (16.16).
   * @param       y   The matrix Y coordinate (16.16).
   * @param       w   The matrix W coordinate (2.30).
   */
  Matrix(int32_t a, int32_t b, int32_t u, int32_t c, int32_t d, int32_t v,
         int32_t x, int32_t y, int32_t w);

  /*!
   * @function    Matrix
   * @abstract    Copy constructor.
   * @param       o   The object to copy from.
   */
  Matrix(const Matrix& o);

  /*!
   * @function    Matrix
   * @abstract    Move constructor.
   * @param       o   The object to move from.
   */
  Matrix(Matrix&& o) noexcept;

  /*!
   * @function    ~Matrix
   * @abstract    Destructor.
   */
  virtual ~Matrix() override;

  /*!
   * @function    operator=
   * @abstract    Assignment operator.
   * @param       o   The object to assign from.
   */
  Matrix& operator=(Matrix o);

  /*!
   * @function    GetName
   * @abstract    Gets the object's name.
   * @result      The object's name
   * @see         DisplayableObject
   */
  std::string GetName() const override;

  /*!
   * @function    GetA
   * @abstract    Gets the matrix A coordinate.
   * @result      The matrix A coordinate (stored as 16.16 fixed point).
   */
  double GetA() const;

  /*!
   * @function    GetB
   * @abstract    Gets the matrix B coordinate.
   * @result      The matrix B coordinate (stored as 16.16 fixed point).
   */
  double GetB() const;

  /*!
   * @function    GetU
   * @abstract    Gets the matrix U coordinate.
   * @result      The matrix U coordinate (stored as 2.30 fixed point).
   */
  double GetU() const;

  /*!
   * @function    GetC
   * @abstract    Gets the matrix C coordinate.
   * @result      The matrix C coordinate (stored as 16.16 fixed point).
   */
  double GetC() const;

  /*!
   * @function    GetD
   * @abstract    Gets the matrix D coordinate.
   * @result      The matrix D coordinate (stored as 16.16 fixed point).
   */
  double GetD() const;

  /*!
   * @function    GetV
   * @abstract    Gets the matrix V coordinate.
   * @result      The matrix V coordinate (stored as 2.30 fixed point).
   */
  double GetV() const;

  /*!
   * @function    GetX
   * @abstract    Gets the matrix X coordinate.
   * @result      The matrix X coordinate (stored as 16.16 fixed point).
   */
  double GetX() const;

  /*!
   * @function    GetY
   * @abstract    Gets the matrix Y coordinate.
   * @result      The matrix Y coordinate (stored as 16.16 fixed point).
   */
  double GetY() const;

  /*!
   * @function    GetW
   * @abstract    Gets the matrix W coordinate.
   * @result      The matrix W coordinate (stored as 2.30 fixed point).
   */
  double GetW() const;

  /*!
   * @function    SetA
   * @abstract    Sets the matrix A coordinate.
   * @param       value   The matrix A coordinate to set (stored as 16.16
   *                      fixed point, rounded and saturated).
   */
  void SetA(double value);

  /*!
   * @function    SetB
   * @abstract    Sets the matrix B coordinate.
   * @param       value   The matrix B coordinate to set (stored as 16.16
   *                      fixed point, rounded and saturated).
   */
  void SetB(double value);

  /*!
   * @function    SetU
   * @abstract    Sets the matrix U coordinate.
   * @param       value   The matrix U coordinate to set (stored as 2.30
   *                      fixed point, rounded and saturated).
   */
  void SetU(double value);

  /*!
   * @function    SetC
   * @abstract    Sets the matrix C coordinate.
   * @param       value   The matrix C coordinate to set (stored as 16.16
   *                      fixed point, rounded and saturated).
   */
  void SetC(double value);

  /*!
   * @function    SetD
   * @abstract    Sets the matrix D coordinate.
   * @param       value   The matrix D coordinate to set (stored as 16.16
   *                      fixed point, rounded and saturated).
   */
  void SetD(double value);

  /*!
   * @function    SetV
   * @abstract    Sets the matrix V coordinate.
   * @param       value   The matrix V coordinate to set (stored as 2.30
   *                      fixed point, rounded and saturated).
   */
  void SetV(double value);

  /*!
   * @function    SetX
   * @abstract    Sets the matrix X coordinate.
   * @param       value   The matrix X coordinate to set (stored as 16.16
   *                      fixed point, rounded and saturated).
   */
  void SetX(double value);

  /*!
   * @function    SetY
   * @abstract    Sets the matrix Y coordinate.
   * @param       value   The matrix Y coordinate to set (stored as 16.16
   *                      fixed point, rounded and saturated).
   */
  void SetY(double value);

  /*!
   * @function    SetW
   * @abstract    Sets the matrix W coordinate.
   * @param       value   The matrix W coordinate to set (stored as 2.30
   *                      fixed point, rounded and saturated).
   */
  void SetW(double value);

  /*!
   * @function    WriteDescription
   * @abstract    Writes the matrix description to an output stream.
   * @param       os              The output stream in which to write the
   * description.
   * @param       indentLevel     The requested indent level.
   * @see         DisplayableObject
   */
  void WriteDescription(std::ostream& os,
                        std::size_t indentLevel) const override;

  /*!
   * @function    GetDisplayableProperties
   * @abstract    Gets the matrix displayable properties.
   * @result      The matrix displayable properties.
   * @see         DisplayableObject
   */
  virtual std::vector<std::pair<std::string, std::string> >
  GetDisplayableProperties() const override;

  /*!
   * @function    swap
   * @abstract    Swap two objects.
   * @param       o1  The first object to swap.
   * @param       o2  The second object to swap.
   */
  ISOBMFF_EXPORT friend void swap(Matrix& o1, Matrix& o2);

 private:
  class IMPL;

  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_MATRIX_HPP */

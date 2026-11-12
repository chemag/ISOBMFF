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
 * @file        Matrix.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <FixedPoint.hpp>
#include <Matrix.hpp>

namespace ISOBMFF {
class Matrix::IMPL {
 public:
  IMPL();
  IMPL(int32_t a, int32_t b, int32_t u, int32_t c, int32_t d, int32_t v,
       int32_t x, int32_t y, int32_t w);
  IMPL(const IMPL& o);
  ~IMPL();

  int32_t _a;
  int32_t _b;
  int32_t _u;
  int32_t _c;
  int32_t _d;
  int32_t _v;
  int32_t _x;
  int32_t _y;
  int32_t _w;
};

Matrix::Matrix() : impl(std::make_unique<IMPL>()) {}

Matrix::Matrix(int32_t a, int32_t b, int32_t u, int32_t c, int32_t d, int32_t v,
               int32_t x, int32_t y, int32_t w)
    : impl(std::make_unique<IMPL>(a, b, u, c, d, v, x, y, w)) {}

Matrix::Matrix(const Matrix& o) : impl(std::make_unique<IMPL>(*(o.impl))) {}

Matrix::Matrix(Matrix&& o) noexcept : impl(std::move(o.impl)) {
  o.impl = nullptr;
}

Matrix::~Matrix() {}

Matrix& Matrix::operator=(Matrix o) {
  swap(*(this), o);

  return *(this);
}

void swap(Matrix& o1, Matrix& o2) {
  using std::swap;

  swap(o1.impl, o2.impl);
}

std::string Matrix::GetName() const { return "Matrix"; }

double Matrix::GetA() const { return FixedPoint::ToDouble(this->impl->_a, 16); }

double Matrix::GetB() const { return FixedPoint::ToDouble(this->impl->_b, 16); }

double Matrix::GetU() const { return FixedPoint::ToDouble(this->impl->_u, 30); }

double Matrix::GetC() const { return FixedPoint::ToDouble(this->impl->_c, 16); }

double Matrix::GetD() const { return FixedPoint::ToDouble(this->impl->_d, 16); }

double Matrix::GetV() const { return FixedPoint::ToDouble(this->impl->_v, 30); }

double Matrix::GetX() const { return FixedPoint::ToDouble(this->impl->_x, 16); }

double Matrix::GetY() const { return FixedPoint::ToDouble(this->impl->_y, 16); }

double Matrix::GetW() const { return FixedPoint::ToDouble(this->impl->_w, 30); }

void Matrix::SetA(double value) {
  this->impl->_a = FixedPoint::FromDouble<int32_t>(value, 16);
}

void Matrix::SetB(double value) {
  this->impl->_b = FixedPoint::FromDouble<int32_t>(value, 16);
}

void Matrix::SetU(double value) {
  this->impl->_u = FixedPoint::FromDouble<int32_t>(value, 30);
}

void Matrix::SetC(double value) {
  this->impl->_c = FixedPoint::FromDouble<int32_t>(value, 16);
}

void Matrix::SetD(double value) {
  this->impl->_d = FixedPoint::FromDouble<int32_t>(value, 16);
}

void Matrix::SetV(double value) {
  this->impl->_v = FixedPoint::FromDouble<int32_t>(value, 30);
}

void Matrix::SetX(double value) {
  this->impl->_x = FixedPoint::FromDouble<int32_t>(value, 16);
}

void Matrix::SetY(double value) {
  this->impl->_y = FixedPoint::FromDouble<int32_t>(value, 16);
}

void Matrix::SetW(double value) {
  this->impl->_w = FixedPoint::FromDouble<int32_t>(value, 30);
}

void Matrix::WriteDescription(std::ostream& os, std::size_t indentLevel) const {
  std::string i(indentLevel * 4, ' ');

  os << i << "{ a = " << this->GetA() << ", b = " << this->GetB()
     << ", u = " << this->GetU() << ", c = " << this->GetC()
     << ", d = " << this->GetD() << ", v = " << this->GetV()
     << ", x = " << this->GetX() << ", y = " << this->GetY()
     << ", w = " << this->GetW() << " }";
}

std::vector<std::pair<std::string, std::string> >
Matrix::GetDisplayableProperties() const {
  return {
      {"a", std::to_string(this->GetA())}, {"b", std::to_string(this->GetB())},
      {"u", std::to_string(this->GetU())}, {"c", std::to_string(this->GetC())},
      {"d", std::to_string(this->GetD())}, {"v", std::to_string(this->GetV())},
      {"x", std::to_string(this->GetX())}, {"y", std::to_string(this->GetY())},
      {"w", std::to_string(this->GetW())},
  };
}

Matrix::IMPL::IMPL() : IMPL(0, 0, 0, 0, 0, 0, 0, 0, 0) {}

Matrix::IMPL::IMPL(int32_t a, int32_t b, int32_t u, int32_t c, int32_t d,
                   int32_t v, int32_t x, int32_t y, int32_t w)
    : _a(a), _b(b), _u(u), _c(c), _d(d), _v(v), _x(x), _y(y), _w(w) {}

Matrix::IMPL::IMPL(const IMPL& o)
    : _a(o._a),
      _b(o._b),
      _u(o._u),
      _c(o._c),
      _d(o._d),
      _v(o._v),
      _x(o._x),
      _y(o._y),
      _w(o._w) {}

Matrix::IMPL::~IMPL() {}
}  // namespace ISOBMFF

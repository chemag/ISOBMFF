/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @header      FixedPoint.hpp
 */

#ifndef ISOBMFF_FIXED_POINT_HPP
#define ISOBMFF_FIXED_POINT_HPP

#include <cmath>
#include <cstdint>
#include <limits>

namespace ISOBMFF {
// Conversions between fixed-point fields (e.g. 16.16, 8.8, or 2.30 in
// ISO/IEC 14496-12) and double. A fixed-point field with fractional_bits
// fractional bits stores value * 2^fractional_bits as an integer of the
// field's type. Every fixed-point value of 32 bits or fewer converts to a
// double exactly.
namespace FixedPoint {

// Returns raw / 2^fractional_bits.
inline double ToDouble(int64_t raw, int fractional_bits) {
  return std::ldexp(static_cast<double>(raw), -fractional_bits);
}

// Returns value * 2^fractional_bits, rounded to the nearest integer and
// saturated to the range of T (NaN converts to 0).
template <typename T>
T FromDouble(double value, int fractional_bits) {
  const double scaled = std::round(std::ldexp(value, fractional_bits));
  if (std::isnan(scaled)) {
    return 0;
  }
  if (scaled <= static_cast<double>(std::numeric_limits<T>::min())) {
    return std::numeric_limits<T>::min();
  }
  if (scaled >= static_cast<double>(std::numeric_limits<T>::max())) {
    return std::numeric_limits<T>::max();
  }
  return static_cast<T>(scaled);
}

}  // namespace FixedPoint
}  // namespace ISOBMFF

#endif /* ISOBMFF_FIXED_POINT_HPP */

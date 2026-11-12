/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <FixedPoint.hpp>
#include <cmath>
#include <cstdint>
#include <limits>

namespace ISOBMFF {

TEST(ISOBMFFFixedPointTest, ToDouble) {
  EXPECT_DOUBLE_EQ(FixedPoint::ToDouble(0x00010000, 16), 1.0);
  EXPECT_DOUBLE_EQ(FixedPoint::ToDouble(0x00018000, 16), 1.5);
  EXPECT_DOUBLE_EQ(FixedPoint::ToDouble(static_cast<int32_t>(0xffff0000), 16),
                   -1.0);
  EXPECT_DOUBLE_EQ(FixedPoint::ToDouble(0x0100, 8), 1.0);
  EXPECT_DOUBLE_EQ(FixedPoint::ToDouble(0x40000000, 30), 1.0);
  // 48000 Hz plus half a hertz
  EXPECT_DOUBLE_EQ(FixedPoint::ToDouble((48000u << 16) | 0x8000u, 16), 48000.5);
  // the largest unsigned 16.16 value converts exactly
  EXPECT_EQ(FixedPoint::ToDouble(UINT32_MAX, 16), 65535.0 + 65535.0 / 65536.0);
}

TEST(ISOBMFFFixedPointTest, FromDouble) {
  EXPECT_EQ(FixedPoint::FromDouble<int32_t>(1.0, 16), 0x00010000);
  EXPECT_EQ(FixedPoint::FromDouble<int32_t>(-1.0, 16),
            static_cast<int32_t>(0xffff0000));
  EXPECT_EQ(FixedPoint::FromDouble<int32_t>(1.0, 30), 0x40000000);
  EXPECT_EQ(FixedPoint::FromDouble<uint32_t>(48000.5, 16),
            (48000u << 16) | 0x8000u);
  EXPECT_EQ(FixedPoint::FromDouble<int16_t>(0.5, 8), 0x0080);
  // rounds to the nearest representable value
  EXPECT_EQ(FixedPoint::FromDouble<int16_t>(1.0 + 1.0 / 1024.0, 8), 0x0100);
  EXPECT_EQ(FixedPoint::FromDouble<int16_t>(1.0 + 3.0 / 1024.0, 8), 0x0101);
  // saturates to the range of the field
  EXPECT_EQ(FixedPoint::FromDouble<int16_t>(1000.0, 8), INT16_MAX);
  EXPECT_EQ(FixedPoint::FromDouble<int16_t>(-1000.0, 8), INT16_MIN);
  EXPECT_EQ(FixedPoint::FromDouble<uint32_t>(-1.0, 16), 0u);
  EXPECT_EQ(FixedPoint::FromDouble<uint32_t>(1e12, 16), UINT32_MAX);
  EXPECT_EQ(FixedPoint::FromDouble<uint16_t>(
                std::numeric_limits<double>::infinity(), 8),
            UINT16_MAX);
  EXPECT_EQ(FixedPoint::FromDouble<int32_t>(std::nan(""), 16), 0);
}

TEST(ISOBMFFFixedPointTest, RoundTrip) {
  // every raw value survives raw -> double -> raw
  for (int32_t raw : {INT32_MIN, -0x10000, -1, 0, 1, 0x10000, INT32_MAX}) {
    EXPECT_EQ(
        FixedPoint::FromDouble<int32_t>(FixedPoint::ToDouble(raw, 16), 16),
        raw);
    EXPECT_EQ(
        FixedPoint::FromDouble<int32_t>(FixedPoint::ToDouble(raw, 30), 30),
        raw);
  }
}

}  // namespace ISOBMFF

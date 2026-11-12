/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various

namespace ISOBMFF {

class ISOBMFFMatrixTest : public ::testing::Test {
 public:
  ISOBMFFMatrixTest() {}
  ~ISOBMFFMatrixTest() override {}
};

// Matrix is part of the mvhd and tkhd boxes, not a registered box, so it has
// no fuzzer of its own (it is fuzzed through MVHD_fuzzer and TKHD_fuzzer).
TEST_F(ISOBMFFMatrixTest, TestMatrixRotation90) {
  // a 90-degree rotation of a 1080-pixel-wide image: {0,1,0, -1,0,0,
  // 1080,0,1}, as stored in a tkhd box
  const std::vector<uint8_t> buffer = {
      // a = 0 (16.16)
      0x00, 0x00, 0x00, 0x00,
      // b = 1.0 (16.16)
      0x00, 0x01, 0x00, 0x00,
      // u = 0 (2.30)
      0x00, 0x00, 0x00, 0x00,
      // c = -1.0 (16.16)
      0xff, 0xff, 0x00, 0x00,
      // d = 0 (16.16)
      0x00, 0x00, 0x00, 0x00,
      // v = 0 (2.30)
      0x00, 0x00, 0x00, 0x00,
      // x = 1080.0 (16.16)
      0x04, 0x38, 0x00, 0x00,
      // y = 0 (16.16)
      0x00, 0x00, 0x00, 0x00,
      // w = 1.0 (2.30)
      0x40, 0x00, 0x00, 0x00};

  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Matrix matrix;
  ISOBMFF::Error error = stream.ReadMatrix(matrix);
  ASSERT_FALSE(error) << error.GetMessage();

  EXPECT_DOUBLE_EQ(matrix.GetA(), 0.0);
  EXPECT_DOUBLE_EQ(matrix.GetB(), 1.0);
  EXPECT_DOUBLE_EQ(matrix.GetU(), 0.0);
  // the matrix values are signed: 0xffff0000 is -1.0
  EXPECT_DOUBLE_EQ(matrix.GetC(), -1.0);
  EXPECT_DOUBLE_EQ(matrix.GetD(), 0.0);
  EXPECT_DOUBLE_EQ(matrix.GetV(), 0.0);
  EXPECT_DOUBLE_EQ(matrix.GetX(), 1080.0);
  EXPECT_DOUBLE_EQ(matrix.GetY(), 0.0);
  // w is 2.30 fixed point: 0x40000000 is 1.0
  EXPECT_DOUBLE_EQ(matrix.GetW(), 1.0);
}

TEST_F(ISOBMFFMatrixTest, TestMatrixSetters) {
  // the unity matrix, as stored
  ISOBMFF::Matrix matrix(0x00010000, 0, 0, 0, 0x00010000, 0, 0, 0,
                         0x40000000);
  EXPECT_DOUBLE_EQ(matrix.GetA(), 1.0);
  EXPECT_DOUBLE_EQ(matrix.GetD(), 1.0);
  EXPECT_DOUBLE_EQ(matrix.GetW(), 1.0);

  matrix.SetA(-0.5);
  EXPECT_DOUBLE_EQ(matrix.GetA(), -0.5);
  matrix.SetX(-12.25);
  EXPECT_DOUBLE_EQ(matrix.GetX(), -12.25);
  // u, v and w have 30 fractional bits
  matrix.SetU(0.000001);
  EXPECT_NEAR(matrix.GetU(), 0.000001, 1.0 / (1 << 30));
  // 2.30 holds [-2, 2): larger values saturate
  matrix.SetW(3.0);
  EXPECT_DOUBLE_EQ(matrix.GetW(), 2.0 - 1.0 / (1 << 30));
}

}  // namespace ISOBMFF

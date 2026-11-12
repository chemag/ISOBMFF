/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various

namespace ISOBMFF {

class ISOBMFFImageGridTest : public ::testing::Test {
 public:
  ISOBMFFImageGridTest() {}
  ~ISOBMFFImageGridTest() override {}
};

// ImageGrid is the data of a "grid" item, not a box, so it has no fuzzer
// (fuzz/converter.py only handles boxes created by Parser::CreateBox()).
TEST_F(ISOBMFFImageGridTest, TestImageGrid2x2) {
  const std::vector<uint8_t> buffer = {
      // version: 0
      0x00,
      // flags: 0 (16-bit output_width and output_height)
      0x00,
      // rows_minus_one: 1 (2 rows)
      0x01,
      // columns_minus_one: 1 (2 columns)
      0x01,
      // output_width: 4096
      0x10, 0x00,
      // output_height: 3072
      0x0c, 0x00};

  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::ImageGrid grid(stream);

  EXPECT_EQ(grid.GetVersion(), 0);
  EXPECT_EQ(grid.GetFlags(), 0);
  // the values as stored: a 2x2 grid has rows_minus_one and
  // columns_minus_one equal to 1
  EXPECT_EQ(grid.GetRowsMinusOne(), 1);
  EXPECT_EQ(grid.GetColumnsMinusOne(), 1);
  EXPECT_EQ(grid.GetOutputWidth(), 4096u);
  EXPECT_EQ(grid.GetOutputHeight(), 3072u);
}

TEST_F(ISOBMFFImageGridTest, TestImageGrid32BitOutputSize) {
  const std::vector<uint8_t> buffer = {
      // version: 0
      0x00,
      // flags: 1 (32-bit output_width and output_height)
      0x01,
      // rows_minus_one: 0 (1 row)
      0x00,
      // columns_minus_one: 2 (3 columns)
      0x02,
      // output_width: 12288
      0x00, 0x00, 0x30, 0x00,
      // output_height: 4096
      0x00, 0x00, 0x10, 0x00};

  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::ImageGrid grid(stream);

  EXPECT_EQ(grid.GetFlags(), 1);
  EXPECT_EQ(grid.GetRowsMinusOne(), 0);
  EXPECT_EQ(grid.GetColumnsMinusOne(), 2);
  EXPECT_EQ(grid.GetOutputWidth(), 12288u);
  EXPECT_EQ(grid.GetOutputHeight(), 4096u);
}
}  // namespace ISOBMFF

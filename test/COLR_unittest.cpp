/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFCOLRTest : public ::testing::Test {
 public:
  ISOBMFFCOLRTest() {}
  ~ISOBMFFCOLRTest() override {}
};

// A synthetic nclx colr: BT.709 primaries, transfer and matrix, full range.
TEST_F(ISOBMFFCOLRTest, TestCOLRParserNclx) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // colour_type = 'nclx'
      0x6e, 0x63, 0x6c, 0x78,
      // colour_primaries = 1, transfer_characteristics = 1, matrix_coefficients
      // = 1
      0x00, 0x01, 0x00, 0x01, 0x00, 0x01,
      // full_range_flag = 1
      0x80};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("colr");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto colr = std::dynamic_pointer_cast<ISOBMFF::COLR>(box);
  ASSERT_NE(colr, nullptr) << "Failed to cast to COLR";
  EXPECT_EQ(colr->GetColourType(), "nclx");
  EXPECT_EQ(colr->GetColourPrimaries(), 1);
  EXPECT_EQ(colr->GetTransferCharacteristics(), 1);
  EXPECT_EQ(colr->GetMatrixCoefficients(), 1);
  EXPECT_TRUE(colr->GetFullRangeFlag());
}

// A prof colr carrying the first 8 bytes of the ICC profile in
// media/IMG1.HEIC (the profile itself is 548 bytes).
TEST_F(ISOBMFFCOLRTest, TestCOLRParserProf) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {// colour_type = 'prof'
                                        0x70, 0x72, 0x6f, 0x66,
                                        // ICC profile (truncated)
                                        0x00, 0x00, 0x02, 0x24, 0x61, 0x70,
                                        0x70, 0x6c};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("colr");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto colr = std::dynamic_pointer_cast<ISOBMFF::COLR>(box);
  ASSERT_NE(colr, nullptr) << "Failed to cast to COLR";
  EXPECT_EQ(colr->GetColourType(), "prof");
  EXPECT_THAT(
      colr->GetICCProfile(),
      ::testing::ElementsAre(0x00, 0x00, 0x02, 0x24, 0x61, 0x70, 0x70, 0x6c));
}

// A truncated nclx colr (no full_range_flag).
TEST_F(ISOBMFFCOLRTest, TestCOLRParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // colour_type = 'nclx', 3 x 16-bit codes
      0x6e, 0x63, 0x6c, 0x78, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("colr");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

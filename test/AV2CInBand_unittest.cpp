/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFAV2CInBandTest : public ::testing::Test {
 public:
  ISOBMFFAV2CInBandTest() {}
  ~ISOBMFFAV2CInBandTest() override {}
};

// A synthetic av2c (no AV2 encoder output available yet).
TEST_F(ISOBMFFAV2CInBandTest, TestAV2CInBandParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // av2c box size: 10 bytes, box header not included
      // reserved1 = 0 (3 bits), max_num_streams_minus1 = 1 (2 bits),
      // max_profile_idc = 2 (5 bits), max_level_idx = 13 (5 bits),
      // max_tier = 1 (1 bit): 000 01 00010 01101 1
      0x08, 0x9b
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("av2c");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto av2c = std::dynamic_pointer_cast<ISOBMFF::AV2CInBand>(box);
  ASSERT_NE(av2c, nullptr) << "Failed to cast to AV2CInBand";

  EXPECT_EQ(av2c->GetName(), "av2c");
  EXPECT_EQ(av2c->GetReserved1(), 0);
  EXPECT_EQ(av2c->GetMaxNumStreamsMinus1(), 1);
  EXPECT_EQ(av2c->GetMaxProfileIdc(), 2);
  EXPECT_EQ(av2c->GetMaxLevelIdx(), 13);
  EXPECT_EQ(av2c->GetMaxTier(), 1);
}

// A truncated av2c (1 of its 2 bytes) must be reported as an error.
TEST_F(ISOBMFFAV2CInBandTest, TestAV2CInBandParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      0x08
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("av2c");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

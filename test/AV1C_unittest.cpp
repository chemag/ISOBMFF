/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFAV1CTest : public ::testing::Test {
 public:
  ISOBMFFAV1CTest() {}
  ~ISOBMFFAV1CTest() override {}
};

TEST_F(ISOBMFFAV1CTest, TestAV1CParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // following example input is from an ffmpeg-generated (libsvtav1) .mp4
      // file (av1C box size: 25 bytes, box header not included)
      // marker = 1, version = 1
      0x81,
      // seq_profile = 0, seq_level_idx_0 = 8
      0x08,
      // seq_tier_0 = 0, high_bitdepth = 0, twelve_bit = 0, monochrome = 0,
      // chroma_subsampling_x = 1, chroma_subsampling_y = 1,
      // chroma_sample_position = 1
      0x0d,
      // initial_presentation_delay_present = 0
      0x00,
      // configOBUs: a sequence header OBU (13 bytes)
      0x0a, 0x0b, 0x00, 0x00, 0x00, 0x42, 0xa7, 0xbf,
      0xe3, 0xe0, 0x17, 0xc0, 0x12
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("av1C");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  // Validate AV1C box
  auto av1c = std::dynamic_pointer_cast<ISOBMFF::AV1C>(box);
  ASSERT_NE(av1c, nullptr) << "Failed to cast to AV1C";

  EXPECT_EQ(av1c->GetMarker(), 1);
  EXPECT_EQ(av1c->GetVersion(), 1);
  EXPECT_EQ(av1c->GetSeqProfile(), 0);
  EXPECT_EQ(av1c->GetSeqLevelIdx0(), 8);
  EXPECT_EQ(av1c->GetSeqTier0(), 0);
  EXPECT_EQ(av1c->GetHighBitdepth(), 0);
  EXPECT_EQ(av1c->GetTwelveBit(), 0);
  EXPECT_EQ(av1c->GetMonochrome(), 0);
  EXPECT_EQ(av1c->GetChromaSubsamplingX(), 1);
  EXPECT_EQ(av1c->GetChromaSubsamplingY(), 1);
  EXPECT_EQ(av1c->GetChromaSamplePosition(), 1);
  EXPECT_EQ(av1c->GetInitialPresentationDelayPresent(), 0);
  EXPECT_EQ(av1c->GetInitialPresentationDelayMinusOne(), 0);
  EXPECT_THAT(
      av1c->GetConfigOBUs(),
      ::testing::ElementsAreArray({0x0a, 0x0b, 0x00, 0x00, 0x00, 0x42, 0xa7,
                                   0xbf, 0xe3, 0xe0, 0x17, 0xc0, 0x12}));
}

// A synthetic av1C exercising the fields the previous test leaves at 0:
// profile 1 (high), tier 1, 10-bit 4:4:4, and an initial presentation delay.
TEST_F(ISOBMFFAV1CTest, TestAV1CParserInitialPresentationDelay) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // marker = 1, version = 1
      0x81,
      // seq_profile = 1, seq_level_idx_0 = 13
      0x2d,
      // seq_tier_0 = 1, high_bitdepth = 1, twelve_bit = 0, monochrome = 0,
      // chroma_subsampling_x = 0, chroma_subsampling_y = 0,
      // chroma_sample_position = 0
      0xc0,
      // initial_presentation_delay_present = 1,
      // initial_presentation_delay_minus_one = 3
      0x13
      // clang-format on
  };
  // (no configOBUs)

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("av1C");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto av1c = std::dynamic_pointer_cast<ISOBMFF::AV1C>(box);
  ASSERT_NE(av1c, nullptr) << "Failed to cast to AV1C";

  EXPECT_EQ(av1c->GetMarker(), 1);
  EXPECT_EQ(av1c->GetVersion(), 1);
  EXPECT_EQ(av1c->GetSeqProfile(), 1);
  EXPECT_EQ(av1c->GetSeqLevelIdx0(), 13);
  EXPECT_EQ(av1c->GetSeqTier0(), 1);
  EXPECT_EQ(av1c->GetHighBitdepth(), 1);
  EXPECT_EQ(av1c->GetTwelveBit(), 0);
  EXPECT_EQ(av1c->GetMonochrome(), 0);
  EXPECT_EQ(av1c->GetChromaSubsamplingX(), 0);
  EXPECT_EQ(av1c->GetChromaSubsamplingY(), 0);
  EXPECT_EQ(av1c->GetChromaSamplePosition(), 0);
  EXPECT_EQ(av1c->GetInitialPresentationDelayPresent(), 1);
  EXPECT_EQ(av1c->GetInitialPresentationDelayMinusOne(), 3);
  EXPECT_TRUE(av1c->GetConfigOBUs().empty());
}

// A truncated av1C (2 of the 4 fixed bytes) must be reported as an error.
TEST_F(ISOBMFFAV1CTest, TestAV1CParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      0x81, 0x08
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("av1C");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

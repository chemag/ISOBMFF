/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFMP4ATest : public ::testing::Test {
 public:
  ISOBMFFMP4ATest() {}
  ~ISOBMFFMP4ATest() override {}
};

TEST_F(ISOBMFFMP4ATest, TestMP4AParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // following example input is from a sample .mp4 video file
      // mp4a size: 75 bytes
      // 0x00, 0x00, 0x00, 0x4b,
      // mp4a
      // 0x6d, 0x70, 0x34, 0x61,
      // mp4a content:
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x02, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00,
      0xbb, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x27,
      0x65, 0x73, 0x64, 0x73, 0x00, 0x00, 0x00, 0x00,
      0x03, 0x19, 0x00, 0x00, 0x00, 0x04, 0x11, 0x40,
      0x15, 0x00, 0x03, 0x00, 0x00, 0x01, 0xf4, 0x00,
      0x00, 0x01, 0xf4, 0x00, 0x05, 0x02, 0x11, 0x90,
      0x06, 0x01, 0x02
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("mp4a");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  // Validate MP4A box
  auto mp4a = std::dynamic_pointer_cast<ISOBMFF::MP4A>(box);
  ASSERT_NE(mp4a, nullptr) << "Failed to cast to MP4A";

  // Validate version and flags
  EXPECT_EQ(mp4a->GetVersion(), 0);
  EXPECT_EQ(mp4a->GetFlags(), 0x00000000);

  // Validate channel count
  EXPECT_EQ(mp4a->GetChannelCount(), 2);
  // Validate sample size
  EXPECT_EQ(mp4a->GetSampleSize(), 16);
  // Validate sample rate (raw)
  EXPECT_EQ(mp4a->GetSampleRateRaw(), 3145728000);
  // Validate sample rate
  EXPECT_DOUBLE_EQ(mp4a->GetSampleRate(), 48000.0);
}

// samplerate is an unsigned 16.16 fixed-point field: a fractional rate
// keeps its fraction (GetSampleRate() used to return a float that dropped
// precision)
TEST_F(ISOBMFFMP4ATest, TestMP4ASampleRate) {
  ISOBMFF::MP4A mp4a;
  mp4a.SetSampleRateRaw((44100u << 16) | 0x8000u);
  EXPECT_DOUBLE_EQ(mp4a.GetSampleRate(), 44100.5);
  mp4a.SetSampleRate(22050.25);
  EXPECT_EQ(mp4a.GetSampleRateRaw(), (22050u << 16) | 0x4000u);
  // the largest 16.16 value is just under 65536 Hz: higher rates saturate
  // (they are carried in a SamplingRateBox instead)
  mp4a.SetSampleRate(96000.0);
  EXPECT_EQ(mp4a.GetSampleRateRaw(), UINT32_MAX);
}
}  // namespace ISOBMFF

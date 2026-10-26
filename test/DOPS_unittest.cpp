/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFDOPSTest : public ::testing::Test {
 public:
  ISOBMFFDOPSTest() {}
  ~ISOBMFFDOPSTest() override {}
};

TEST_F(ISOBMFFDOPSTest, TestDOPSParserFamily0) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // following example input is from an ffmpeg-generated (libopus) .mp4
      // file (dOps box size: 19 bytes, box header not included)
      // Version = 0, OutputChannelCount = 2
      0x00, 0x02,
      // PreSkip = 312
      0x01, 0x38,
      // InputSampleRate = 48000
      0x00, 0x00, 0xbb, 0x80,
      // OutputGain = 0
      0x00, 0x00,
      // ChannelMappingFamily = 0
      0x00};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("dOps");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  // Validate DOPS box
  auto dops = std::dynamic_pointer_cast<ISOBMFF::DOPS>(box);
  ASSERT_NE(dops, nullptr) << "Failed to cast to DOPS";

  EXPECT_EQ(dops->GetVersion(), 0);
  EXPECT_EQ(dops->GetOutputChannelCount(), 2);
  EXPECT_EQ(dops->GetPreSkip(), 312);
  EXPECT_EQ(dops->GetInputSampleRate(), 48000u);
  EXPECT_EQ(dops->GetOutputGain(), 0);
  EXPECT_EQ(dops->GetChannelMappingFamily(), 0);
  EXPECT_EQ(dops->GetStreamCount(), 0);
  EXPECT_EQ(dops->GetCoupledCount(), 0);
  EXPECT_TRUE(dops->GetChannelMapping().empty());
}

TEST_F(ISOBMFFDOPSTest, TestDOPSParserFamily1) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {// 5.1 surround (Vorbis channel order)
                                        // Version = 0, OutputChannelCount = 6
                                        0x00, 0x06,
                                        // PreSkip = 312
                                        0x01, 0x38,
                                        // InputSampleRate = 44100
                                        0x00, 0x00, 0xac, 0x44,
                                        // OutputGain = -256 (-1 dB in Q7.8)
                                        0xff, 0x00,
                                        // ChannelMappingFamily = 1
                                        0x01,
                                        // StreamCount = 4, CoupledCount = 2
                                        0x04, 0x02,
                                        // ChannelMapping = [0, 4, 1, 2, 3, 5]
                                        0x00, 0x04, 0x01, 0x02, 0x03, 0x05};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("dOps");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  // Validate DOPS box
  auto dops = std::dynamic_pointer_cast<ISOBMFF::DOPS>(box);
  ASSERT_NE(dops, nullptr) << "Failed to cast to DOPS";

  EXPECT_EQ(dops->GetVersion(), 0);
  EXPECT_EQ(dops->GetOutputChannelCount(), 6);
  EXPECT_EQ(dops->GetPreSkip(), 312);
  EXPECT_EQ(dops->GetInputSampleRate(), 44100u);
  EXPECT_EQ(dops->GetOutputGain(), -256);
  EXPECT_EQ(dops->GetChannelMappingFamily(), 1);
  EXPECT_EQ(dops->GetStreamCount(), 4);
  EXPECT_EQ(dops->GetCoupledCount(), 2);
  EXPECT_THAT(dops->GetChannelMapping(),
              ::testing::ElementsAre(0, 4, 1, 2, 3, 5));

  // Validate displayable properties
  auto props = dops->GetDisplayableProperties();
  EXPECT_EQ(props.back().first, "Channel Mapping");
  EXPECT_EQ(props.back().second, "0, 4, 1, 2, 3, 5");
}

TEST_F(ISOBMFFDOPSTest, TestDOPSParserTruncatedMapping) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // ChannelMappingFamily = 1 with a truncated ChannelMapping
      0x00, 0x06, 0x01, 0x38, 0x00, 0x00, 0xbb, 0x80,
      0x00, 0x00, 0x01, 0x04, 0x02, 0x00, 0x04};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("dOps");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

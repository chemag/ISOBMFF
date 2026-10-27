/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFPIXITest : public ::testing::Test {
 public:
  ISOBMFFPIXITest() {}
  ~ISOBMFFPIXITest() override {}
};

TEST_F(ISOBMFFPIXITest, TestPIXIParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {// pixi version 0, flags 0
                                        0x00, 0x00, 0x00, 0x00,
                                        // num_channels: 3
                                        0x03,
                                        // bits_per_channel: 8, 8, 8
                                        0x08, 0x08, 0x08};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("pixi");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error) << error.GetMessage();
  auto pixi = std::dynamic_pointer_cast<ISOBMFF::PIXI>(box);
  ASSERT_NE(pixi, nullptr) << "Failed to cast to PIXI";
  auto channels = pixi->GetChannels();
  ASSERT_EQ(channels.size(), 3);
  for (const auto& channel : channels) {
    EXPECT_EQ(channel->GetBitsPerChannel(), 8);
  }
}

// A channel count larger than the data must stop at the end of the data
// instead of adding empty channels until the count is reached.
TEST_F(ISOBMFFPIXITest, TestPIXIChannelCountExceedsData) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {// pixi version 0, flags 0
                                        0x00, 0x00, 0x00, 0x00,
                                        // num_channels: 255
                                        0xff,
                                        // bits_per_channel: 10, 10
                                        0x0a, 0x0a};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("pixi");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error) << "expected an error for a truncated channel list";
  auto pixi = std::dynamic_pointer_cast<ISOBMFF::PIXI>(box);
  ASSERT_NE(pixi, nullptr) << "Failed to cast to PIXI";
  auto channels = pixi->GetChannels();
  ASSERT_EQ(channels.size(), 2);
  EXPECT_EQ(channels[0]->GetBitsPerChannel(), 10);
  EXPECT_EQ(channels[1]->GetBitsPerChannel(), 10);
}

}  // namespace ISOBMFF

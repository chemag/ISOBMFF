/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFAV2SampleEntryTest : public ::testing::Test {
 public:
  ISOBMFFAV2SampleEntryTest() {}
  ~ISOBMFFAV2SampleEntryTest() override {}
};

// A synthetic OutOfBandAV2SampleEntry ("av2E") carrying an av2C box.
TEST_F(ISOBMFFAV2SampleEntryTest, TestAV2SampleEntryOutOfBand) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // av2E box size: 105 bytes, box header not included
      // VisualSampleEntry: data_reference_index = 1, 1920x1080,
      // 72 dpi, frame_count = 1, empty compressorname, depth = 24
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x07, 0x80, 0x04, 0x38, 0x00, 0x48, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x18, 0xff, 0xff,
      // av2C (19 bytes): 2 OBUs, 3 and 2 bytes
      0x00, 0x00, 0x00, 0x13, 0x61, 0x76, 0x32, 0x43, 0x00, 0x01, 0x03, 0x08,
      0x00, 0x2a, 0x82, 0x80, 0x00, 0x0c, 0x01};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("av2E");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto entry = std::dynamic_pointer_cast<ISOBMFF::AV2SampleEntry>(box);
  ASSERT_NE(entry, nullptr) << "Failed to cast to AV2SampleEntry";

  EXPECT_EQ(entry->GetName(), "av2E");
  EXPECT_EQ(entry->GetDataReferenceIndex(), 1);
  EXPECT_EQ(entry->GetWidth(), 1920);
  EXPECT_EQ(entry->GetHeight(), 1080);
  EXPECT_EQ(entry->GetHorizResolutionRaw(), 0x00480000u);
  EXPECT_DOUBLE_EQ(entry->GetHorizResolution(), 72.0);
  EXPECT_EQ(entry->GetVertResolutionRaw(), 0x00480000u);
  EXPECT_DOUBLE_EQ(entry->GetVertResolution(), 72.0);
  EXPECT_EQ(entry->GetFrameCount(), 1);
  EXPECT_EQ(entry->GetDepth(), 24);

  auto av2c = entry->GetTypedBox<ISOBMFF::AV2COutOfBand>("av2C");
  ASSERT_NE(av2c, nullptr) << "Failed to find the av2C box";
  EXPECT_EQ(av2c->GetConfigOBUs().size(), 2);
}

// A synthetic InBandAV2SampleEntry ("av2e") carrying an av2c box.
TEST_F(ISOBMFFAV2SampleEntryTest, TestAV2SampleEntryInBand) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // av2e box size: 96 bytes, box header not included
      // VisualSampleEntry: data_reference_index = 1, 1920x1080,
      // 72 dpi, frame_count = 1, empty compressorname, depth = 24
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x07, 0x80, 0x04, 0x38, 0x00, 0x48, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x18, 0xff, 0xff,
      // av2c (10 bytes): max_num_streams_minus1 = 1, max_profile_idc = 2,
      // max_level_idx = 13, max_tier = 1
      0x00, 0x00, 0x00, 0x0a, 0x61, 0x76, 0x32, 0x63, 0x08, 0x9b};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("av2e");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto entry = std::dynamic_pointer_cast<ISOBMFF::AV2SampleEntry>(box);
  ASSERT_NE(entry, nullptr) << "Failed to cast to AV2SampleEntry";

  EXPECT_EQ(entry->GetName(), "av2e");
  EXPECT_EQ(entry->GetWidth(), 1920);
  EXPECT_EQ(entry->GetHeight(), 1080);

  auto av2c = entry->GetTypedBox<ISOBMFF::AV2CInBand>("av2c");
  ASSERT_NE(av2c, nullptr) << "Failed to find the av2c box";
  EXPECT_EQ(av2c->GetMaxProfileIdc(), 2);
  EXPECT_EQ(av2c->GetMaxLevelIdx(), 13);
  // the out-of-band configuration box is not there
  EXPECT_EQ(entry->GetBox("av2C"), nullptr);
}
}  // namespace ISOBMFF

/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFOPUSTest : public ::testing::Test {
 public:
  ISOBMFFOPUSTest() {}
  ~ISOBMFFOPUSTest() override {}
};

TEST_F(ISOBMFFOPUSTest, TestOPUSParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // following example input is from an ffmpeg-generated (libopus) .mp4
      // file (Opus box size: 75 bytes, box header not included)
      // reserved (6 bytes) + data_reference_index (2 bytes) = 1
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
      // reserved (8 bytes)
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      // channelcount (2 bytes) = 2, samplesize (2 bytes) = 16
      0x00, 0x02, 0x00, 0x10,
      // pre_defined (2 bytes) + reserved (2 bytes)
      0x00, 0x00, 0x00, 0x00,
      // samplerate (4 bytes) = 48000 << 16
      0xbb, 0x80, 0x00, 0x00,
      // dOps box (size: 19 bytes)
      0x00, 0x00, 0x00, 0x13, 0x64, 0x4f, 0x70, 0x73, 0x00, 0x02, 0x01, 0x38,
      0x00, 0x00, 0xbb, 0x80, 0x00, 0x00, 0x00,
      // btrt box (size: 20 bytes)
      0x00, 0x00, 0x00, 0x14, 0x62, 0x74, 0x72, 0x74, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x31, 0x2a, 0x00, 0x01, 0x31, 0x2a};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("Opus");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  // Validate OPUS box
  auto opus = std::dynamic_pointer_cast<ISOBMFF::OPUS>(box);
  ASSERT_NE(opus, nullptr) << "Failed to cast to OPUS";
  EXPECT_EQ(opus->GetName(), "Opus");

  EXPECT_EQ(opus->GetDataReferenceIndex(), 1);
  EXPECT_EQ(opus->GetChannelCount(), 2);
  EXPECT_EQ(opus->GetSampleSize(), 16);
  EXPECT_EQ(opus->GetSampleRateRaw(), 3145728000u);
  EXPECT_FLOAT_EQ(opus->GetSampleRate(), 48000);

  // Validate child boxes
  auto boxes = opus->GetBoxes();
  ASSERT_EQ(boxes.size(), 2u);
  EXPECT_EQ(boxes[0]->GetName(), "dOps");
  EXPECT_EQ(boxes[1]->GetName(), "btrt");

  // Validate dOps box
  auto dops = opus->GetTypedBox<ISOBMFF::DOPS>("dOps");
  ASSERT_NE(dops, nullptr) << "Failed to get DOPS";
  EXPECT_EQ(dops->GetVersion(), 0);
  EXPECT_EQ(dops->GetOutputChannelCount(), 2);
  EXPECT_EQ(dops->GetPreSkip(), 312);
  EXPECT_EQ(dops->GetInputSampleRate(), 48000u);
  EXPECT_EQ(dops->GetOutputGain(), 0);
  EXPECT_EQ(dops->GetChannelMappingFamily(), 0);
}
}  // namespace ISOBMFF

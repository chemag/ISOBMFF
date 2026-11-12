/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFAV01Test : public ::testing::Test {
 public:
  ISOBMFFAV01Test() {}
  ~ISOBMFFAV01Test() override {}
};

TEST_F(ISOBMFFAV01Test, TestAV01Parser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // following example input is from an ffmpeg-generated (libsvtav1) .mp4
      // file (av01 box size: 141 bytes, box header not included)
      // reserved (6 bytes), data_reference_index = 1
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
      // pre_defined, reserved, pre_defined (16 bytes)
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      // width = 1920, height = 800
      0x07, 0x80, 0x03, 0x20,
      // horizresolution, vertresolution = 72 dpi
      0x00, 0x48, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00,
      // reserved, frame_count = 1
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
      // compressorname: "Lavc61.11.100 libsvtav1"
      0x17, 0x4c, 0x61, 0x76, 0x63, 0x36, 0x31, 0x2e,
      0x31, 0x31, 0x2e, 0x31, 0x30, 0x30, 0x20, 0x6c,
      0x69, 0x62, 0x73, 0x76, 0x74, 0x61, 0x76, 0x31,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      // depth = 24, pre_defined = -1
      0x00, 0x18, 0xff, 0xff,
      // av1C (25 bytes)
      0x00, 0x00, 0x00, 0x19, 0x61, 0x76, 0x31, 0x43,
      0x81, 0x08, 0x0d, 0x00, 0x0a, 0x0b, 0x00, 0x00,
      0x00, 0x42, 0xa7, 0xbf, 0xe3, 0xe0, 0x17, 0xc0,
      0x12,
      // fiel (10 bytes)
      0x00, 0x00, 0x00, 0x0a, 0x66, 0x69, 0x65, 0x6c,
      0x01, 0x00,
      // btrt (20 bytes)
      0x00, 0x00, 0x00, 0x14, 0x62, 0x74, 0x72, 0x74,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x62, 0x3f,
      0x00, 0x0e, 0x62, 0x3f
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("av01");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  // Validate AV01 box
  auto av01 = std::dynamic_pointer_cast<ISOBMFF::AV01>(box);
  ASSERT_NE(av01, nullptr) << "Failed to cast to AV01";

  EXPECT_EQ(av01->GetDataReferenceIndex(), 1);
  EXPECT_EQ(av01->GetWidth(), 1920);
  EXPECT_EQ(av01->GetHeight(), 800);
  EXPECT_EQ(av01->GetHorizResolutionRaw(), 0x00480000u);
  EXPECT_DOUBLE_EQ(av01->GetHorizResolution(), 72.0);
  EXPECT_EQ(av01->GetVertResolutionRaw(), 0x00480000u);
  EXPECT_DOUBLE_EQ(av01->GetVertResolution(), 72.0);
  EXPECT_EQ(av01->GetFrameCount(), 1);
  EXPECT_EQ(av01->GetDepth(), 24);

  // the av1C child box is parsed as an AV1C
  auto av1c = av01->GetTypedBox<ISOBMFF::AV1C>("av1C");
  ASSERT_NE(av1c, nullptr) << "Failed to find the av1C box";
  EXPECT_EQ(av1c->GetSeqProfile(), 0);
  EXPECT_EQ(av1c->GetSeqLevelIdx0(), 8);
  EXPECT_EQ(av1c->GetConfigOBUs().size(), 13);
}
}  // namespace ISOBMFF

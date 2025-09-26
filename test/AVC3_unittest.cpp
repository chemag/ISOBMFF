/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <ISOBMFF.hpp>                  // for various
#include <BinaryDataStream.hpp> // for BinaryDataStream
#include <Parser.hpp>           // for Parser

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ISOBMFF {

class ISOBMFFAVC3Test : public ::testing::Test {
public:
  ISOBMFFAVC3Test() {}
  ~ISOBMFFAVC3Test() override {}
};

TEST_F(ISOBMFFAVC3Test, TestAVC3Parser) {
  // fuzzer::conv: data
  const std::vector<uint8_t> &buffer = {
      // following example input is from a sample .mp4 video file
      // avc3 size: 182 bytes (same data as avc1 test since they're identical)
      // 0x00, 0x00, 0x00, 0xb6,
      // avc3
      // 0x61, 0x76, 0x63, 0x33,
      // avc3 content:
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x06, 0xc0, 0x09, 0x00, 0x00, 0x48, 0x00, 0x00,
      0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x15, 0x4c, 0x61, 0x76, 0x63, 0x36,
      0x31, 0x2e, 0x31, 0x39, 0x2e, 0x31, 0x30, 0x31,
      0x20, 0x6c, 0x69, 0x62, 0x78, 0x32, 0x36, 0x34,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x18, 0xff, 0xff, 0x00, 0x00,
      0x00, 0x39, 0x61, 0x76, 0x63, 0x43, 0x01, 0x64,
      0x00, 0x34, 0xff, 0xe1, 0x00, 0x1c, 0x67, 0x64,
      0x00, 0x34, 0xac, 0xd9, 0x40, 0x6c, 0x01, 0x21,
      0xa6, 0xa1, 0x22, 0x41, 0x28, 0x00, 0x00, 0x03,
      0x00, 0x08, 0x00, 0x00, 0x07, 0x80, 0x78, 0xc1,
      0x8c, 0xb0, 0x01, 0x00, 0x06, 0x68, 0xeb, 0xe3,
      0xcb, 0x22, 0xc0, 0xfd, 0xf8, 0xf8, 0x00, 0x00,
      0x00, 0x00, 0x13, 0x63, 0x6f, 0x6c, 0x72, 0x6e,
      0x63, 0x6c, 0x78, 0x00, 0x09, 0x00, 0x12, 0x00,
      0x09, 0x00, 0x00, 0x00, 0x00, 0x14, 0x62, 0x74,
      0x72, 0x74, 0x00, 0x00, 0x00, 0x00, 0x00, 0x9a,
      0x9e, 0xc2, 0x00, 0x00, 0x00, 0x00
    };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("avc3");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  // Validate AVC3 box
  auto avc3 = std::dynamic_pointer_cast<ISOBMFF::AVC3>(box);
  ASSERT_NE(avc3, nullptr) << "Failed to cast to AVC3";

  // Validate version and flags
  EXPECT_EQ(avc3->GetVersion(), 0);
  EXPECT_EQ(avc3->GetFlags(), 0);

  // Validate data reference index
  EXPECT_EQ(avc3->GetDataReferenceIndex(), 1);

  // Validate width and height (should be identical to AVC1 since they share implementation)
  EXPECT_EQ(avc3->GetWidth(), 1728);  // 0x06C0
  EXPECT_EQ(avc3->GetHeight(), 2304); // 0x0900

  // Validate horizontal and vertical resolution
  EXPECT_EQ(avc3->GetHorizResolution(), 0x00480000); // 72 dpi in 16.16 fixed-point
  EXPECT_EQ(avc3->GetVertResolution(), 0x00480000);  // 72 dpi in 16.16 fixed-point

  // Validate frame count
  EXPECT_EQ(avc3->GetFrameCount(), 1);

  // Validate compressor name
  EXPECT_EQ(avc3->GetCompressorName(), "\x15Lavc61.19.101 libx264");

  // Validate depth
  EXPECT_EQ(avc3->GetDepth(), 24);
}

TEST_F(ISOBMFFAVC3Test, TestAVC3BoxType) {
  // Test that AVC3 box is created with correct name
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("avc3");

  ASSERT_NE(box, nullptr) << "Failed to create avc3 box";
  EXPECT_EQ(box->GetName(), "avc3") << "Box name should be avc3";

  // Test that it can be cast to AVC3
  auto avc3 = std::dynamic_pointer_cast<ISOBMFF::AVC3>(box);
  ASSERT_NE(avc3, nullptr) << "Failed to cast to AVC3";

  // Test that it can also be cast to AVC1 (since AVC3 inherits from AVC1)
  auto avc1 = std::dynamic_pointer_cast<ISOBMFF::AVC1>(box);
  ASSERT_NE(avc1, nullptr) << "Failed to cast to AVC1 (inheritance test)";
}
} // namespace ISOBMFF
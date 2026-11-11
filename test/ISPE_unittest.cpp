/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFISPETest : public ::testing::Test {
 public:
  ISOBMFFISPETest() {}
  ~ISOBMFFISPETest() override {}
};

// following example input is from an Apple iPhone HEIC file (media/IMG1.HEIC).
TEST_F(ISOBMFFISPETest, TestISPEParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // ispe box size: 20 bytes, box header not included
      // version = 0, flags = 0
      0x00, 0x00, 0x00, 0x00,
      // image_width = 4032, image_height = 3024
      0x00, 0x00, 0x0f, 0xc0, 0x00, 0x00, 0x0b, 0xd0};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("ispe");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto ispe = std::dynamic_pointer_cast<ISOBMFF::ISPE>(box);
  ASSERT_NE(ispe, nullptr) << "Failed to cast to ISPE";
  EXPECT_EQ(ispe->GetDisplayWidth(), 4032u);
  EXPECT_EQ(ispe->GetDisplayHeight(), 3024u);
}

// A truncated ispe (no image_height).
TEST_F(ISOBMFFISPETest, TestISPEParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // version = 0, flags = 0, image_width = 4032
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xc0};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("ispe");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

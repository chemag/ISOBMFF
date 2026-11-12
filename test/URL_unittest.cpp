/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFURLTest : public ::testing::Test {
 public:
  ISOBMFFURLTest() {}
  ~ISOBMFFURLTest() override {}
};

TEST_F(ISOBMFFURLTest, TestURLParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // following example input is from a sample .mp4 video file
      // url  size: 12 bytes
      // 0x00, 0x00, 0x00, 0x0c,
      // url
      // 0x75, 0x72, 0x6c, 0x20,
      // url  content:
      0x00, 0x00, 0x00, 0x01,
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("url ");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  // Validate URL box
  auto url = std::dynamic_pointer_cast<ISOBMFF::URL>(box);
  ASSERT_NE(url, nullptr) << "Failed to cast to URL";

  // Validate version and flags
  EXPECT_EQ(url->GetVersion(), 0);
  EXPECT_EQ(url->GetFlags(), 0x00000001);
  // self-contained: no location string
  EXPECT_EQ(url->GetLocation(), "");
}

// A url box that points to another file: its location is kept.
TEST_F(ISOBMFFURLTest, TestURLParserLocation) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 0, flags = 0 (media data in another file)
      0x00, 0x00, 0x00, 0x00,
      // location = "file:a.mp4"
      0x66, 0x69, 0x6c, 0x65, 0x3a, 0x61, 0x2e, 0x6d,
      0x70, 0x34, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("url ");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto url = std::dynamic_pointer_cast<ISOBMFF::URL>(box);
  ASSERT_NE(url, nullptr) << "Failed to cast to URL";
  EXPECT_EQ(url->GetFlags(), 0u);
  EXPECT_EQ(url->GetLocation(), "file:a.mp4");
}
}  // namespace ISOBMFF

/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFPITMTest : public ::testing::Test {
 public:
  ISOBMFFPITMTest() {}
  ~ISOBMFFPITMTest() override {}
};

// following example input is from an Apple iPhone HEIC file (media/IMG1.HEIC).
TEST_F(ISOBMFFPITMTest, TestPITMParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // pitm box size: 14 bytes, box header not included
      // version = 0, flags = 0
      0x00, 0x00, 0x00, 0x00,
      // item_ID = 49
      0x00, 0x31
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("pitm");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto pitm = std::dynamic_pointer_cast<ISOBMFF::PITM>(box);
  ASSERT_NE(pitm, nullptr) << "Failed to cast to PITM";
  EXPECT_EQ(pitm->GetVersion(), 0);
  EXPECT_EQ(pitm->GetItemID(), 49u);
}

// A synthetic version 1 pitm, with a 32-bit item_ID.
TEST_F(ISOBMFFPITMTest, TestPITMParserVersion1) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 1, flags = 0
      0x01, 0x00, 0x00, 0x00,
      // item_ID = 65536
      0x00, 0x01, 0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("pitm");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto pitm = std::dynamic_pointer_cast<ISOBMFF::PITM>(box);
  ASSERT_NE(pitm, nullptr) << "Failed to cast to PITM";
  EXPECT_EQ(pitm->GetVersion(), 1);
  EXPECT_EQ(pitm->GetItemID(), 65536u);
}

// A truncated pitm (1 of the 2 item_ID bytes).
TEST_F(ISOBMFFPITMTest, TestPITMParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 0, flags = 0, item_ID truncated
      0x00, 0x00, 0x00, 0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("pitm");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

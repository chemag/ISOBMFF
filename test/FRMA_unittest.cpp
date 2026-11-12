/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFFRMATest : public ::testing::Test {
 public:
  ISOBMFFFRMATest() {}
  ~ISOBMFFFRMATest() override {}
};

// A synthetic frma, as in the sinf of an encrypted avc1 track.
TEST_F(ISOBMFFFRMATest, TestFRMAParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // data_format = 'avc1'
      0x61, 0x76, 0x63, 0x31
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("frma");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto frma = std::dynamic_pointer_cast<ISOBMFF::FRMA>(box);
  ASSERT_NE(frma, nullptr) << "Failed to cast to FRMA";
  EXPECT_EQ(frma->GetDataFormat(), "avc1");
}

// A truncated frma (2 of the 4 data_format bytes).
TEST_F(ISOBMFFFRMATest, TestFRMAParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // data_format truncated
      0x61, 0x76
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("frma");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

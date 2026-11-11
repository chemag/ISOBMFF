/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFIROTTest : public ::testing::Test {
 public:
  ISOBMFFIROTTest() {}
  ~ISOBMFFIROTTest() override {}
};

// following example input is from an Apple iPhone HEIC file (media/IMG1.HEIC).
TEST_F(ISOBMFFIROTTest, TestIROTParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // irot box size: 9 bytes, box header not included
      // reserved = 0, angle = 3 (270 degrees anti-clockwise)
      0x03};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("irot");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto irot = std::dynamic_pointer_cast<ISOBMFF::IROT>(box);
  ASSERT_NE(irot, nullptr) << "Failed to cast to IROT";
  EXPECT_EQ(irot->GetAngle(), 3);
}

// A synthetic irot with its 6 reserved bits set: only angle is kept.
TEST_F(ISOBMFFIROTTest, TestIROTParserReservedBits) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {// reserved = 0x3f, angle = 1
                                        0xfd};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("irot");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto irot = std::dynamic_pointer_cast<ISOBMFF::IROT>(box);
  ASSERT_NE(irot, nullptr) << "Failed to cast to IROT";
  EXPECT_EQ(irot->GetAngle(), 1);
}

// An empty irot.
TEST_F(ISOBMFFIROTTest, TestIROTParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // (no data)
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("irot");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

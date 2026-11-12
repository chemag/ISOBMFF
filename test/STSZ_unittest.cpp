/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFSTSZTest : public ::testing::Test {
 public:
  ISOBMFFSTSZTest() {}
  ~ISOBMFFSTSZTest() override {}
};

TEST_F(ISOBMFFSTSZTest, TestSTSZParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // the first 5 entries of the video track stsz in media/MOV1.MOV,
      // with sample_count set to 5
      // stsz size: 40 bytes
      // 0x00, 0x00, 0x00, 0x28,
      // stsz
      // 0x73, 0x74, 0x73, 0x7a,
      // stsz content:
      // version (1 byte) and flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // sample_size: 0 (the sizes are in the table)
      0x00, 0x00, 0x00, 0x00,
      // sample_count: 5
      0x00, 0x00, 0x00, 0x05,
      // entry_size[0..4]
      0x00, 0x02, 0x91, 0x0b, 0x00, 0x01, 0xd4, 0x68,
      0x00, 0x01, 0x31, 0x94, 0x00, 0x00, 0x99, 0x67,
      0x00, 0x00, 0x3f, 0xd4
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("stsz");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error) << error.GetMessage();
  auto stsz = std::dynamic_pointer_cast<ISOBMFF::STSZ>(box);
  ASSERT_NE(stsz, nullptr) << "Failed to cast to STSZ";

  EXPECT_EQ(stsz->GetSampleSize(), 0);
  EXPECT_EQ(stsz->GetSampleCount(), 5);
  ASSERT_EQ(stsz->GetEntryCount(), 5);
  EXPECT_EQ(stsz->GetEntrySize(0), 168203);
  EXPECT_EQ(stsz->GetEntrySize(1), 119912);
  EXPECT_EQ(stsz->GetEntrySize(2), 78228);
  EXPECT_EQ(stsz->GetEntrySize(3), 39271);
  EXPECT_EQ(stsz->GetEntrySize(4), 16340);
}

TEST_F(ISOBMFFSTSZTest, TestSTSZParserConstantSize) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // the stsz of the first timed-metadata track in media/MOV1.MOV
      // stsz size: 20 bytes
      // 0x00, 0x00, 0x00, 0x14,
      // stsz
      // 0x73, 0x74, 0x73, 0x7a,
      // stsz content:
      // version (1 byte) and flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // sample_size: 8 (every sample is 8 bytes: there is no table)
      0x00, 0x00, 0x00, 0x08,
      // sample_count: 1
      0x00, 0x00, 0x00, 0x01
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("stsz");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error) << error.GetMessage();
  auto stsz = std::dynamic_pointer_cast<ISOBMFF::STSZ>(box);
  ASSERT_NE(stsz, nullptr) << "Failed to cast to STSZ";

  EXPECT_EQ(stsz->GetSampleSize(), 8);
  EXPECT_EQ(stsz->GetSampleCount(), 1);
  EXPECT_EQ(stsz->GetEntryCount(), 0);
}

TEST_F(ISOBMFFSTSZTest, TestSTSZParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // sample_count says 5 samples, but the table holds only 3 entries
      // stsz content:
      // version (1 byte) and flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // sample_size: 0 (the sizes are in the table)
      0x00, 0x00, 0x00, 0x00,
      // sample_count: 5
      0x00, 0x00, 0x00, 0x05,
      // entry_size[0..2]
      0x00, 0x02, 0x91, 0x0b, 0x00, 0x01, 0xd4, 0x68,
      0x00, 0x01, 0x31, 0x94
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("stsz");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  // a table that does not back sample_count is an error
  EXPECT_TRUE(error);
  EXPECT_EQ(error.GetCode(), ISOBMFF::ErrorCode::InsufficientData);
}
}  // namespace ISOBMFF

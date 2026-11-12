/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFSTZ2Test : public ::testing::Test {
 public:
  ISOBMFFSTZ2Test() {}
  ~ISOBMFFSTZ2Test() override {}
};

TEST_F(ISOBMFFSTZ2Test, TestSTZ2Parser16Bit) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // the first 5 entries of the audio track stsz in media/MOV1.MOV,
      // re-encoded as a stz2 with 16-bit entries (no file in our test
      // corpora uses stz2)
      // stz2 size: 30 bytes
      // 0x00, 0x00, 0x00, 0x1e,
      // stz2
      // 0x73, 0x74, 0x7a, 0x32,
      // stz2 content:
      // version (1 byte) and flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // reserved (3 bytes), field_size: 16
      0x00, 0x00, 0x00, 0x10,
      // sample_count: 5
      0x00, 0x00, 0x00, 0x05,
      // entry_size[0..4]
      0x00, 0x04, 0x00, 0xb3, 0x01, 0xfc, 0x01, 0x2c,
      0x01, 0x21
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("stz2");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error) << error.GetMessage();
  auto stz2 = std::dynamic_pointer_cast<ISOBMFF::STZ2>(box);
  ASSERT_NE(stz2, nullptr) << "Failed to cast to STZ2";

  EXPECT_EQ(stz2->GetFieldSize(), 16);
  EXPECT_EQ(stz2->GetSampleCount(), 5);
  ASSERT_EQ(stz2->GetEntryCount(), 5);
  EXPECT_EQ(stz2->GetEntrySize(0), 4);
  EXPECT_EQ(stz2->GetEntrySize(1), 179);
  EXPECT_EQ(stz2->GetEntrySize(2), 508);
  EXPECT_EQ(stz2->GetEntrySize(3), 300);
  EXPECT_EQ(stz2->GetEntrySize(4), 289);
}

TEST_F(ISOBMFFSTZ2Test, TestSTZ2Parser8Bit) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // stz2 content:
      // version (1 byte) and flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // reserved (3 bytes), field_size: 8
      0x00, 0x00, 0x00, 0x08,
      // sample_count: 3
      0x00, 0x00, 0x00, 0x03,
      // entry_size[0..2]
      0x04, 0xb3, 0xff
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("stz2");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error) << error.GetMessage();
  auto stz2 = std::dynamic_pointer_cast<ISOBMFF::STZ2>(box);
  ASSERT_NE(stz2, nullptr) << "Failed to cast to STZ2";

  EXPECT_EQ(stz2->GetFieldSize(), 8);
  EXPECT_EQ(stz2->GetSampleCount(), 3);
  ASSERT_EQ(stz2->GetEntryCount(), 3);
  EXPECT_EQ(stz2->GetEntrySize(0), 4);
  EXPECT_EQ(stz2->GetEntrySize(1), 179);
  EXPECT_EQ(stz2->GetEntrySize(2), 255);
}

TEST_F(ISOBMFFSTZ2Test, TestSTZ2Parser4Bit) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // stz2 content:
      // version (1 byte) and flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // reserved (3 bytes), field_size: 4
      0x00, 0x00, 0x00, 0x04,
      // sample_count: 3 (odd: the last lower nibble is padding)
      0x00, 0x00, 0x00, 0x03,
      // entry_size[0..2]: 1, 15, 7 (and a 0 nibble of padding)
      0x1f, 0x70
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("stz2");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error) << error.GetMessage();
  auto stz2 = std::dynamic_pointer_cast<ISOBMFF::STZ2>(box);
  ASSERT_NE(stz2, nullptr) << "Failed to cast to STZ2";

  EXPECT_EQ(stz2->GetFieldSize(), 4);
  EXPECT_EQ(stz2->GetSampleCount(), 3);
  ASSERT_EQ(stz2->GetEntryCount(), 3);
  EXPECT_EQ(stz2->GetEntrySize(0), 1);
  EXPECT_EQ(stz2->GetEntrySize(1), 15);
  EXPECT_EQ(stz2->GetEntrySize(2), 7);
}

TEST_F(ISOBMFFSTZ2Test, TestSTZ2ParserInvalidFieldSize) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // stz2 content:
      // version (1 byte) and flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // reserved (3 bytes), field_size: 12 (only 4, 8, or 16 are valid)
      0x00, 0x00, 0x00, 0x0c,
      // sample_count: 2
      0x00, 0x00, 0x00, 0x02,
      // 24 bits of entries
      0x00, 0x40, 0x0b
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("stz2");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
  EXPECT_EQ(error.GetCode(), ISOBMFF::ErrorCode::InvalidBoxData);
}

TEST_F(ISOBMFFSTZ2Test, TestSTZ2ParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // sample_count says 5 samples, but the table holds only 3 entries
      // stz2 content:
      // version (1 byte) and flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // reserved (3 bytes), field_size: 16
      0x00, 0x00, 0x00, 0x10,
      // sample_count: 5
      0x00, 0x00, 0x00, 0x05,
      // entry_size[0..2]
      0x00, 0x04, 0x00, 0xb3, 0x01, 0xfc
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("stz2");

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

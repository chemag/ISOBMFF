/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFELSTTest : public ::testing::Test {
 public:
  ISOBMFFELSTTest() {}
  ~ISOBMFFELSTTest() override {}
};

TEST_F(ISOBMFFELSTTest, TestELSTParserVersion0) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // following example input is from MOV1.MOV test file
      // elst size: 28 bytes
      // 0x00, 0x00, 0x00, 0x1c,
      // elst
      // 0x65, 0x6c, 0x73, 0x74,
      // elst content (version 0):
      // version (1 byte) + flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // entry_count (4 bytes) = 1
      0x00, 0x00, 0x00, 0x01,
      // edit_duration (4 bytes) = 6344 (0x18c8)
      0x00, 0x00, 0x18, 0xc8,
      // media_time (4 bytes) = 0
      0x00, 0x00, 0x00, 0x00,
      // media_rate_integer (2 bytes) = 1
      0x00, 0x01,
      // media_rate_fraction (2 bytes) = 0
      0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("elst");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  // Validate ELST box
  auto elst = std::dynamic_pointer_cast<ISOBMFF::ELST>(box);
  ASSERT_NE(elst, nullptr) << "Failed to cast to ELST";

  // Validate version and flags
  EXPECT_EQ(elst->GetVersion(), 0);
  EXPECT_EQ(elst->GetFlags(), 0u);

  // Validate entry count
  EXPECT_EQ(elst->GetEntryCount(), 1u);

  // Validate entry values
  EXPECT_EQ(elst->GetEditDuration(0), 6344u);
  EXPECT_EQ(elst->GetMediaTime(0), 0);
  EXPECT_EQ(elst->GetMediaRateInteger(0), 1);
  EXPECT_EQ(elst->GetMediaRateFraction(0), 0);
  EXPECT_DOUBLE_EQ(elst->GetMediaRate(0), 1.0);

  // Validate displayable properties
  auto displayableProperties = elst->GetDisplayableProperties();
  EXPECT_EQ(displayableProperties[0].first, "Version");
  EXPECT_EQ(displayableProperties[0].second, "0");
  EXPECT_EQ(displayableProperties[1].first, "Flags");
  EXPECT_EQ(displayableProperties[1].second, "0x00000000");
  EXPECT_EQ(displayableProperties[2].first, "Entry count");
  EXPECT_EQ(displayableProperties[2].second, "1");
  EXPECT_EQ(displayableProperties[3].first, "Edit Duration");
  EXPECT_EQ(displayableProperties[3].second, "6344");
  EXPECT_EQ(displayableProperties[4].first, "Media Time");
  EXPECT_EQ(displayableProperties[4].second, "0");
  EXPECT_EQ(displayableProperties[5].first, "Media Rate Integer");
  EXPECT_EQ(displayableProperties[5].second, "1");
  EXPECT_EQ(displayableProperties[6].first, "Media Rate Fraction");
  EXPECT_EQ(displayableProperties[6].second, "0");
  EXPECT_EQ(displayableProperties[7].first, "Media Rate");
  EXPECT_EQ(displayableProperties[7].second, std::to_string(1.0));
}

TEST_F(ISOBMFFELSTTest, TestELSTParserVersion0MultipleEntries) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // elst content (version 0) with 2 entries:
      // version (1 byte) + flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // entry_count (4 bytes) = 2
      0x00, 0x00, 0x00, 0x02,
      // Entry 1:
      // edit_duration (4 bytes) = 1000
      0x00, 0x00, 0x03, 0xe8,
      // media_time (4 bytes) = -1 (empty edit)
      0xff, 0xff, 0xff, 0xff,
      // media_rate_integer (2 bytes) = 1
      0x00, 0x01,
      // media_rate_fraction (2 bytes) = 0
      0x00, 0x00,
      // Entry 2:
      // edit_duration (4 bytes) = 5000
      0x00, 0x00, 0x13, 0x88,
      // media_time (4 bytes) = 500
      0x00, 0x00, 0x01, 0xf4,
      // media_rate_integer (2 bytes) = 2
      0x00, 0x02,
      // media_rate_fraction (2 bytes) = 0x8000 (0.5)
      0x80, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("elst");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  // Validate ELST box
  auto elst = std::dynamic_pointer_cast<ISOBMFF::ELST>(box);
  ASSERT_NE(elst, nullptr) << "Failed to cast to ELST";

  // Validate entry count
  EXPECT_EQ(elst->GetEntryCount(), 2u);

  // Validate entry 1 (empty edit with media_time = -1)
  EXPECT_EQ(elst->GetEditDuration(0), 1000u);
  EXPECT_EQ(elst->GetMediaTime(0), -1);
  EXPECT_EQ(elst->GetMediaRateInteger(0), 1);
  EXPECT_EQ(elst->GetMediaRateFraction(0), 0);

  // Validate entry 2
  EXPECT_EQ(elst->GetEditDuration(1), 5000u);
  EXPECT_EQ(elst->GetMediaTime(1), 500);
  EXPECT_EQ(elst->GetMediaRateInteger(1), 2);
  // 0x8000 as signed int16 is -32768
  EXPECT_EQ(elst->GetMediaRateFraction(1), -32768);
  // media_rate is 0x00028000 (16.16): 2.5
  EXPECT_DOUBLE_EQ(elst->GetMediaRate(1), 2.5);
}

TEST_F(ISOBMFFELSTTest, TestELSTParserVersion1) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // elst content (version 1) with 64-bit values:
      // version (1 byte) = 1 + flags (3 bytes)
      0x01, 0x00, 0x00, 0x00,
      // entry_count (4 bytes) = 1
      0x00, 0x00, 0x00, 0x01,
      // edit_duration (8 bytes) = 0x0000000100000000 (4294967296)
      0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      // media_time (8 bytes) = 1000
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xe8,
      // media_rate_integer (2 bytes) = 1
      0x00, 0x01,
      // media_rate_fraction (2 bytes) = 0
      0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("elst");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  // Validate ELST box
  auto elst = std::dynamic_pointer_cast<ISOBMFF::ELST>(box);
  ASSERT_NE(elst, nullptr) << "Failed to cast to ELST";

  // Validate version
  EXPECT_EQ(elst->GetVersion(), 1);

  // Validate entry count
  EXPECT_EQ(elst->GetEntryCount(), 1u);

  // Validate entry values (64-bit)
  EXPECT_EQ(elst->GetEditDuration(0), 0x0000000100000000ULL);
  EXPECT_EQ(elst->GetMediaTime(0), 1000);
  EXPECT_EQ(elst->GetMediaRateInteger(0), 1);
  EXPECT_EQ(elst->GetMediaRateFraction(0), 0);
}

TEST_F(ISOBMFFELSTTest, TestELSTParserTestMp4) {
  // Data extracted from /tmp/test.mp4
  // This file has 1 elst box with 2 entries
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // elst content (version 0) with 2 entries from test.mp4:
      // version (1 byte) + flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // entry_count (4 bytes) = 2
      0x00, 0x00, 0x00, 0x02,
      // Entry 1: empty edit
      // edit_duration (4 bytes) = 4320 (0x10e0)
      0x00, 0x00, 0x10, 0xe0,
      // media_time (4 bytes) = -1 (0xffffffff)
      0xff, 0xff, 0xff, 0xff,
      // media_rate_integer (2 bytes) = 1
      0x00, 0x01,
      // media_rate_fraction (2 bytes) = 0
      0x00, 0x00,
      // Entry 2: actual media
      // edit_duration (4 bytes) = 155336 (0x25ec8)
      0x00, 0x02, 0x5e, 0xc8,
      // media_time (4 bytes) = 0
      0x00, 0x00, 0x00, 0x00,
      // media_rate_integer (2 bytes) = 1
      0x00, 0x01,
      // media_rate_fraction (2 bytes) = 0
      0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("elst");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end
  EXPECT_FALSE(error) << "Parse error: " << error.GetMessage();

  // Validate ELST box
  auto elst = std::dynamic_pointer_cast<ISOBMFF::ELST>(box);
  ASSERT_NE(elst, nullptr) << "Failed to cast to ELST";

  // Validate entry count
  EXPECT_EQ(elst->GetEntryCount(), 2u);

  // Validate entry 1 (empty edit)
  EXPECT_EQ(elst->GetEditDuration(0), 4320u);
  EXPECT_EQ(elst->GetMediaTime(0), -1);
  EXPECT_EQ(elst->GetMediaRateInteger(0), 1);
  EXPECT_EQ(elst->GetMediaRateFraction(0), 0);

  // Validate entry 2
  EXPECT_EQ(elst->GetEditDuration(1), 155336u);
  EXPECT_EQ(elst->GetMediaTime(1), 0);
  EXPECT_EQ(elst->GetMediaRateInteger(1), 1);
  EXPECT_EQ(elst->GetMediaRateFraction(1), 0);
}

TEST_F(ISOBMFFELSTTest, TestELSTParserStackedMp4Track1) {
  // Data extracted from /tmp/stacked.mp4 - first track (video)
  // This file has 2 elst boxes, this is the first one with 1 entry
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // elst content (version 0) with 1 entry from stacked.mp4 track 1:
      // version (1 byte) + flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // entry_count (4 bytes) = 1
      0x00, 0x00, 0x00, 0x01,
      // edit_duration (4 bytes) = 15934 (0x3e3e)
      0x00, 0x00, 0x3e, 0x3e,
      // media_time (4 bytes) = 0
      0x00, 0x00, 0x00, 0x00,
      // media_rate_integer (2 bytes) = 1
      0x00, 0x01,
      // media_rate_fraction (2 bytes) = 0
      0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("elst");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end
  EXPECT_FALSE(error) << "Parse error: " << error.GetMessage();

  // Validate ELST box
  auto elst = std::dynamic_pointer_cast<ISOBMFF::ELST>(box);
  ASSERT_NE(elst, nullptr) << "Failed to cast to ELST";

  // Validate entry count
  EXPECT_EQ(elst->GetEntryCount(), 1u);

  // Validate entry
  EXPECT_EQ(elst->GetEditDuration(0), 15934u);
  EXPECT_EQ(elst->GetMediaTime(0), 0);
  EXPECT_EQ(elst->GetMediaRateInteger(0), 1);
  EXPECT_EQ(elst->GetMediaRateFraction(0), 0);
}

TEST_F(ISOBMFFELSTTest, TestELSTParserStackedMp4Track2) {
  // Data extracted from /tmp/stacked.mp4 - second track (audio)
  // This file has 2 elst boxes, this is the second one with 2 entries
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // elst content (version 0) with 2 entries from stacked.mp4 track 2:
      // version (1 byte) + flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // entry_count (4 bytes) = 2
      0x00, 0x00, 0x00, 0x02,
      // Entry 1: empty edit
      // edit_duration (4 bytes) = 432 (0x1b0)
      0x00, 0x00, 0x01, 0xb0,
      // media_time (4 bytes) = -1 (0xffffffff)
      0xff, 0xff, 0xff, 0xff,
      // media_rate_integer (2 bytes) = 1
      0x00, 0x01,
      // media_rate_fraction (2 bytes) = 0
      0x00, 0x00,
      // Entry 2: actual media
      // edit_duration (4 bytes) = 15534 (0x3cae)
      0x00, 0x00, 0x3c, 0xae,
      // media_time (4 bytes) = 0
      0x00, 0x00, 0x00, 0x00,
      // media_rate_integer (2 bytes) = 1
      0x00, 0x01,
      // media_rate_fraction (2 bytes) = 0
      0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("elst");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end
  EXPECT_FALSE(error) << "Parse error: " << error.GetMessage();

  // Validate ELST box
  auto elst = std::dynamic_pointer_cast<ISOBMFF::ELST>(box);
  ASSERT_NE(elst, nullptr) << "Failed to cast to ELST";

  // Validate entry count
  EXPECT_EQ(elst->GetEntryCount(), 2u);

  // Validate entry 1 (empty edit)
  EXPECT_EQ(elst->GetEditDuration(0), 432u);
  EXPECT_EQ(elst->GetMediaTime(0), -1);
  EXPECT_EQ(elst->GetMediaRateInteger(0), 1);
  EXPECT_EQ(elst->GetMediaRateFraction(0), 0);

  // Validate entry 2
  EXPECT_EQ(elst->GetEditDuration(1), 15534u);
  EXPECT_EQ(elst->GetMediaTime(1), 0);
  EXPECT_EQ(elst->GetMediaRateInteger(1), 1);
  EXPECT_EQ(elst->GetMediaRateFraction(1), 0);
}

TEST_F(ISOBMFFELSTTest, TestELSTMediaRate) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // elst content (version 0) with 2 entries:
      // version (1 byte) + flags (3 bytes)
      0x00, 0x00, 0x00, 0x00,
      // entry_count (4 bytes) = 2
      0x00, 0x00, 0x00, 0x02,
      // Entry 1: half-speed reverse play
      // edit_duration (4 bytes) = 1000
      0x00, 0x00, 0x03, 0xe8,
      // media_time (4 bytes) = 2000
      0x00, 0x00, 0x07, 0xd0,
      // media_rate (16.16) = -0.5 (0xffff8000):
      // media_rate_integer (2 bytes) = 0xffff
      0xff, 0xff,
      // media_rate_fraction (2 bytes) = 0x8000
      0x80, 0x00,
      // Entry 2: a dwell (media_rate = 0)
      // edit_duration (4 bytes) = 500
      0x00, 0x00, 0x01, 0xf4,
      // media_time (4 bytes) = 2000
      0x00, 0x00, 0x07, 0xd0,
      // media_rate_integer (2 bytes) = 0
      0x00, 0x00,
      // media_rate_fraction (2 bytes) = 0
      0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("elst");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end
  EXPECT_FALSE(error) << "Parse error: " << error.GetMessage();

  auto elst = std::dynamic_pointer_cast<ISOBMFF::ELST>(box);
  ASSERT_NE(elst, nullptr) << "Failed to cast to ELST";
  ASSERT_EQ(elst->GetEntryCount(), 2u);

  // the raw halves, as stored
  EXPECT_EQ(elst->GetMediaRateInteger(0), -1);
  EXPECT_EQ(elst->GetMediaRateFraction(0), -32768);
  // the 16.16 value they form: -1 + 0.5
  EXPECT_DOUBLE_EQ(elst->GetMediaRate(0), -0.5);
  EXPECT_DOUBLE_EQ(elst->GetMediaRate(1), 0.0);
}

}  // namespace ISOBMFF

/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser
#include <chrono>

namespace ISOBMFF {

class ISOBMFFILOCTest : public ::testing::Test {
 public:
  ISOBMFFILOCTest() {}
  ~ISOBMFFILOCTest() override {}
};

TEST_F(ISOBMFFILOCTest, TestILOCParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // iloc version 0, flags 0
      0x00, 0x00, 0x00, 0x00,
      // offset_size: 4, length_size: 4
      0x44,
      // base_offset_size: 0, reserved: 0
      0x00,
      // item_count: 2
      0x00, 0x02,
      // item 1: item_ID: 1, data_reference_index: 0, extent_count: 1
      0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
      // extent_offset: 0x100, extent_length: 0x20
      0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x20,
      // item 2: item_ID: 2, data_reference_index: 0, extent_count: 2
      0x00, 0x02, 0x00, 0x00, 0x00, 0x02,
      // extent_offset: 0x200, extent_length: 0x10
      0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x10,
      // extent_offset: 0x300, extent_length: 0x30
      0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x30};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("iloc");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error) << error.GetMessage();
  auto iloc = std::dynamic_pointer_cast<ISOBMFF::ILOC>(box);
  ASSERT_NE(iloc, nullptr) << "Failed to cast to ILOC";
  EXPECT_EQ(iloc->GetVersion(), 0);
  EXPECT_EQ(iloc->GetOffsetSize(), 4);
  EXPECT_EQ(iloc->GetLengthSize(), 4);
  EXPECT_EQ(iloc->GetBaseOffsetSize(), 0);

  auto items = iloc->GetItems();
  ASSERT_EQ(items.size(), 2);
  EXPECT_EQ(items[0]->GetItemID(), 1);
  ASSERT_EQ(items[0]->GetExtents().size(), 1);
  EXPECT_EQ(items[0]->GetExtents()[0]->GetOffset(), 0x100);
  EXPECT_EQ(items[0]->GetExtents()[0]->GetLength(), 0x20);
  EXPECT_EQ(items[1]->GetItemID(), 2);
  ASSERT_EQ(items[1]->GetExtents().size(), 2);
  EXPECT_EQ(items[1]->GetExtents()[1]->GetOffset(), 0x300);
  EXPECT_EQ(items[1]->GetExtents()[1]->GetLength(), 0x30);
}

// An item count larger than the data (2^32 - 1 here) must stop at the end of
// the data instead of creating empty items until the count is reached.
TEST_F(ISOBMFFILOCTest, TestILOCItemCountExceedsData) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // iloc version 2, flags 0
      0x02, 0x00, 0x00, 0x00,
      // offset_size: 4, length_size: 4
      0x44,
      // base_offset_size: 0, index_size: 0
      0x00,
      // item_count: 0xffffffff
      0xff, 0xff, 0xff, 0xff,
      // item 1: item_ID: 1, construction_method: 0, data_reference_index: 0,
      // extent_count: 1
      0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
      // extent_offset: 0x10, extent_length: 0x20
      0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20};

  auto start = std::chrono::steady_clock::now();
  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("iloc");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end
  auto elapsed = std::chrono::steady_clock::now() - start;

  EXPECT_TRUE(error) << "expected an error for a truncated item list";
  EXPECT_LT(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count(),
            5);
  auto iloc = std::dynamic_pointer_cast<ISOBMFF::ILOC>(box);
  ASSERT_NE(iloc, nullptr) << "Failed to cast to ILOC";
  auto items = iloc->GetItems();
  ASSERT_EQ(items.size(), 1);
  EXPECT_EQ(items[0]->GetItemID(), 1);
  ASSERT_EQ(items[0]->GetExtents().size(), 1);
  EXPECT_EQ(items[0]->GetExtents()[0]->GetOffset(), 0x10);
  EXPECT_EQ(items[0]->GetExtents()[0]->GetLength(), 0x20);
}

// An extent count larger than the data must stop at the end of the data.
TEST_F(ISOBMFFILOCTest, TestILOCExtentCountExceedsData) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // iloc version 0, flags 0
      0x00, 0x00, 0x00, 0x00,
      // offset_size: 4, length_size: 4
      0x44,
      // base_offset_size: 0, reserved: 0
      0x00,
      // item_count: 1
      0x00, 0x01,
      // item 1: item_ID: 1, data_reference_index: 0, extent_count: 0xffff
      0x00, 0x01, 0x00, 0x00, 0xff, 0xff,
      // extent_offset: 0x10, extent_length: 0x20
      0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20};

  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("iloc");
  ASSERT_NE(box, nullptr);
  box->ReadData(parser, stream);

  auto iloc = std::dynamic_pointer_cast<ISOBMFF::ILOC>(box);
  ASSERT_NE(iloc, nullptr) << "Failed to cast to ILOC";
  auto items = iloc->GetItems();
  ASSERT_EQ(items.size(), 1);
  ASSERT_EQ(items[0]->GetExtents().size(), 1);
  EXPECT_EQ(items[0]->GetExtents()[0]->GetOffset(), 0x10);
  EXPECT_EQ(items[0]->GetExtents()[0]->GetLength(), 0x20);
}

// Extents with no index, offset, or length fields take no bytes. A large
// extent count must not create one object per (identical) extent.
TEST_F(ISOBMFFILOCTest, TestILOCZeroSizeExtents) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // iloc version 0, flags 0
      0x00, 0x00, 0x00, 0x00,
      // offset_size: 0, length_size: 0
      0x00,
      // base_offset_size: 0, reserved: 0
      0x00,
      // item_count: 2
      0x00, 0x02,
      // item 1: item_ID: 1, data_reference_index: 0, extent_count: 0xffff
      0x00, 0x01, 0x00, 0x00, 0xff, 0xff,
      // item 2: item_ID: 2, data_reference_index: 0, extent_count: 0xffff
      0x00, 0x02, 0x00, 0x00, 0xff, 0xff};

  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("iloc");
  ASSERT_NE(box, nullptr);
  ISOBMFF::Error error = box->ReadData(parser, stream);
  EXPECT_FALSE(error) << error.GetMessage();

  auto iloc = std::dynamic_pointer_cast<ISOBMFF::ILOC>(box);
  ASSERT_NE(iloc, nullptr) << "Failed to cast to ILOC";
  auto items = iloc->GetItems();
  ASSERT_EQ(items.size(), 2);
  EXPECT_EQ(items[0]->GetExtents().size(), 1);
  EXPECT_EQ(items[1]->GetExtents().size(), 1);
}

// Data ending part-way through an item (1 byte left): the item reads fail
// without consuming the byte, so every further item was empty and the
// loop ran until the item count (65535 here, up to 2^32 - 1 in version 2).
TEST_F(ISOBMFFILOCTest, TestILOCTruncatedItem) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // iloc version 0, flags 0
      0x00, 0x00, 0x00, 0x00,
      // offset_size: 4, length_size: 4
      0x44,
      // base_offset_size: 0, reserved: 0
      0x00,
      // item_count: 0xffff
      0xff, 0xff,
      // item 1: item_ID: 1, data_reference_index: 0, extent_count: 1
      0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
      // extent_offset: 0x10, extent_length: 0x20
      0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20,
      // item 2: truncated (1 byte)
      0x00};

  auto start = std::chrono::steady_clock::now();
  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("iloc");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end
  auto elapsed = std::chrono::steady_clock::now() - start;

  EXPECT_TRUE(error) << "expected an error for a truncated item";
  EXPECT_LT(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count(),
            5);
  auto iloc = std::dynamic_pointer_cast<ISOBMFF::ILOC>(box);
  ASSERT_NE(iloc, nullptr) << "Failed to cast to ILOC";
  auto items = iloc->GetItems();
  ASSERT_EQ(items.size(), 1);
  EXPECT_EQ(items[0]->GetItemID(), 1);
  ASSERT_EQ(items[0]->GetExtents().size(), 1);
}

// Data ending part-way through an extent (3 bytes left): the extent reads
// fail without consuming data, so every further extent was empty.
TEST_F(ISOBMFFILOCTest, TestILOCTruncatedExtent) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // iloc version 0, flags 0
      0x00, 0x00, 0x00, 0x00,
      // offset_size: 4, length_size: 4
      0x44,
      // base_offset_size: 0, reserved: 0
      0x00,
      // item_count: 1
      0x00, 0x01,
      // item 1: item_ID: 1, data_reference_index: 0, extent_count: 0xffff
      0x00, 0x01, 0x00, 0x00, 0xff, 0xff,
      // extent_offset: 0x10, extent_length: 0x20
      0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x20,
      // extent 2: truncated (3 bytes)
      0x00, 0x00, 0x00};

  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("iloc");
  ASSERT_NE(box, nullptr);
  box->ReadData(parser, stream);

  auto iloc = std::dynamic_pointer_cast<ISOBMFF::ILOC>(box);
  ASSERT_NE(iloc, nullptr) << "Failed to cast to ILOC";
  auto items = iloc->GetItems();
  ASSERT_EQ(items.size(), 1);
  ASSERT_EQ(items[0]->GetExtents().size(), 1);
  EXPECT_EQ(items[0]->GetExtents()[0]->GetOffset(), 0x10);
  EXPECT_EQ(items[0]->GetExtents()[0]->GetLength(), 0x20);
}

// Fuzzer-found input (STSD_fuzzer timeout): an stsd box containing an iloc
// box (version 0x69, so a 32-bit item count of 0x1100006a) whose items take
// 4 bytes each, with 1 byte left at the end. Parsing used to run for
// minutes.
TEST_F(ISOBMFFILOCTest, TestILOCTruncatedItemInStsd) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      0x17, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x23, 0x69, 0x6c, 0x6f, 0x63, 0x69, 0x6e, 0x66, 0x65, 0x95, 0x00,
      0x11, 0x00, 0x00, 0x6a, 0x6a, 0x00, 0x6a, 0x6a, 0x63, 0x6a, 0x6a,
      0x13, 0xff, 0x00, 0x00, 0x00, 0x00, 0x6a, 0x00, 0x6a, 0x6a, 0x00,
      0x6a, 0x6a, 0x63, 0x6a, 0x6a, 0x13, 0xff, 0xfd, 0x8b, 0x0f, 0x6a,
      0xce, 0xff, 0xff, 0xff, 0x79, 0x70};

  auto start = std::chrono::steady_clock::now();
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("stsd");
  ASSERT_NE(box, nullptr);
  ISOBMFF::Error error = box->ReadData(parser, stream);
  auto elapsed = std::chrono::steady_clock::now() - start;

  EXPECT_TRUE(error) << "expected an error for the truncated iloc box";
  EXPECT_LT(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count(),
            5);
}

}  // namespace ISOBMFF

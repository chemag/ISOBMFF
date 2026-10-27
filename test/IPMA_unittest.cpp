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

class ISOBMFFIPMATest : public ::testing::Test {
 public:
  ISOBMFFIPMATest() {}
  ~ISOBMFFIPMATest() override {}
};

TEST_F(ISOBMFFIPMATest, TestIPMAParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // ipma version 0, flags 0 (8-bit associations)
      0x00, 0x00, 0x00, 0x00,
      // entry_count: 2
      0x00, 0x00, 0x00, 0x02,
      // entry 1: item_ID: 1, association_count: 2
      0x00, 0x01, 0x02,
      // essential: 1, property_index: 1
      0x81,
      // essential: 0, property_index: 2
      0x02,
      // entry 2: item_ID: 2, association_count: 1
      0x00, 0x02, 0x01,
      // essential: 1, property_index: 3
      0x83};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("ipma");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error) << error.GetMessage();
  auto ipma = std::dynamic_pointer_cast<ISOBMFF::IPMA>(box);
  ASSERT_NE(ipma, nullptr) << "Failed to cast to IPMA";
  auto entries = ipma->GetEntries();
  ASSERT_EQ(entries.size(), 2);
  EXPECT_EQ(entries[0]->GetItemID(), 1);
  auto associations = entries[0]->GetAssociations();
  ASSERT_EQ(associations.size(), 2);
  EXPECT_TRUE(associations[0]->GetEssential());
  EXPECT_EQ(associations[0]->GetPropertyIndex(), 1);
  EXPECT_FALSE(associations[1]->GetEssential());
  EXPECT_EQ(associations[1]->GetPropertyIndex(), 2);
  EXPECT_EQ(entries[1]->GetItemID(), 2);
  ASSERT_EQ(entries[1]->GetAssociations().size(), 1);
  EXPECT_EQ(entries[1]->GetAssociations()[0]->GetPropertyIndex(), 3);
}

// An entry count larger than the data (2^32 - 1 here) must stop at the end
// of the data instead of creating empty entries until the count is reached.
TEST_F(ISOBMFFIPMATest, TestIPMAEntryCountExceedsData) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // ipma version 0, flags 0
      0x00, 0x00, 0x00, 0x00,
      // entry_count: 0xffffffff
      0xff, 0xff, 0xff, 0xff,
      // entry 1: item_ID: 1, association_count: 1, property_index: 1
      0x00, 0x01, 0x01, 0x81};

  auto start = std::chrono::steady_clock::now();
  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("ipma");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end
  auto elapsed = std::chrono::steady_clock::now() - start;

  EXPECT_TRUE(error) << "expected an error for a truncated entry list";
  EXPECT_LT(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count(),
            5);
  auto ipma = std::dynamic_pointer_cast<ISOBMFF::IPMA>(box);
  ASSERT_NE(ipma, nullptr) << "Failed to cast to IPMA";
  auto entries = ipma->GetEntries();
  ASSERT_EQ(entries.size(), 1);
  EXPECT_EQ(entries[0]->GetItemID(), 1);
  ASSERT_EQ(entries[0]->GetAssociations().size(), 1);
}

// Data ending part-way through an entry (1 byte left, less than the 32-bit
// item_ID): the 2^28 declared entries used to be created, each reading
// nothing. Now at most one (partial) entry is created, and the parse fails.
TEST_F(ISOBMFFIPMATest, TestIPMATruncatedEntry) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // ipma version 1 (32-bit item_ID), flags 0
      0x01, 0x00, 0x00, 0x00,
      // entry_count: 0x10000000
      0x10, 0x00, 0x00, 0x00,
      // entry 1: truncated (1 byte)
      0x00};

  auto start = std::chrono::steady_clock::now();
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("ipma");
  ASSERT_NE(box, nullptr);
  ISOBMFF::Error error = box->ReadData(parser, stream);
  auto elapsed = std::chrono::steady_clock::now() - start;

  EXPECT_TRUE(error) << "expected an error for a truncated entry";
  EXPECT_LT(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count(),
            5);
  auto ipma = std::dynamic_pointer_cast<ISOBMFF::IPMA>(box);
  ASSERT_NE(ipma, nullptr) << "Failed to cast to IPMA";
  EXPECT_LE(ipma->GetEntries().size(), 1);
}

// An association count larger than the data must stop at the end of the
// data instead of adding empty associations.
TEST_F(ISOBMFFIPMATest, TestIPMAAssociationCountExceedsData) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // ipma version 0, flags 1 (16-bit associations)
      0x00, 0x00, 0x00, 0x01,
      // entry_count: 1
      0x00, 0x00, 0x00, 0x01,
      // entry 1: item_ID: 1, association_count: 255
      0x00, 0x01, 0xff,
      // essential: 1, property_index: 5
      0x80, 0x05,
      // association 2: truncated (1 byte)
      0x00};

  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("ipma");
  ASSERT_NE(box, nullptr);
  box->ReadData(parser, stream);

  auto ipma = std::dynamic_pointer_cast<ISOBMFF::IPMA>(box);
  ASSERT_NE(ipma, nullptr) << "Failed to cast to IPMA";
  auto entries = ipma->GetEntries();
  ASSERT_EQ(entries.size(), 1);
  auto associations = entries[0]->GetAssociations();
  ASSERT_EQ(associations.size(), 1);
  EXPECT_TRUE(associations[0]->GetEssential());
  EXPECT_EQ(associations[0]->GetPropertyIndex(), 5);
}

// Fuzzer-found input (Parser_fuzzer out-of-memory): an ipma box whose
// version and flags come from file-type text, declaring 0x726f6964 entries
// in a 34-byte box. Parsing used to allocate until it ran out of memory.
TEST_F(ISOBMFFIPMATest, TestIPMAEntryCountInFile) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      0x00, 0x00, 0x00, 0x18, 0x66, 0x74, 0x79, 0x70, 0x6d, 0x61, 0x6e, 0x64,
      0x72, 0x6f, 0x69, 0x64, 0x2e, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6e, 0x6e,
      0x00, 0x00, 0x00, 0x22, 0x69, 0x70, 0x6d, 0x61, 0x6e, 0x64, 0x72, 0x6f,
      0x69, 0x64, 0x2e, 0x76, 0x65, 0x72, 0x73, 0x69, 0x6e, 0x6e, 0x00, 0x00,
      0x18, 0x66, 0x74, 0x79, 0x70, 0x00, 0x00, 0x01, 0x74, 0x00};

  auto start = std::chrono::steady_clock::now();
  ISOBMFF::Parser parser;
  ISOBMFF::Error error = parser.Parse(buffer);
  auto elapsed = std::chrono::steady_clock::now() - start;

  EXPECT_TRUE(error) << "expected an error for the truncated ipma box";
  EXPECT_LT(std::chrono::duration_cast<std::chrono::seconds>(elapsed).count(),
            5);
}

}  // namespace ISOBMFF

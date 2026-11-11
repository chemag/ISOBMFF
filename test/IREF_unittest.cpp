/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFIREFTest : public ::testing::Test {
 public:
  ISOBMFFIREFTest() {}
  ~ISOBMFFIREFTest() override {}
};

// Built from an Apple iPhone HEIC file (media/IMG1.HEIC): its thmb and cdsc
// boxes, and its dimg box cut to the first 4 of its 48 references
// (reference_count adjusted).
TEST_F(ISOBMFFIREFTest, TestIREFParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // version = 0, flags = 0
      0x00, 0x00, 0x00, 0x00,
      // dimg: from_item_ID = 49, reference_count = 4, to_item_ID = 1..4
      0x00, 0x00, 0x00, 0x14, 0x64, 0x69, 0x6d, 0x67, 0x00, 0x31, 0x00, 0x04,
      0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x04,
      // thmb: from_item_ID = 50, reference_count = 1, to_item_ID = 49
      0x00, 0x00, 0x00, 0x0e, 0x74, 0x68, 0x6d, 0x62, 0x00, 0x32, 0x00, 0x01,
      0x00, 0x31,
      // cdsc: from_item_ID = 51, reference_count = 1, to_item_ID = 49
      0x00, 0x00, 0x00, 0x0e, 0x63, 0x64, 0x73, 0x63, 0x00, 0x33, 0x00, 0x01,
      0x00, 0x31};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("iref");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto iref = std::dynamic_pointer_cast<ISOBMFF::IREF>(box);
  ASSERT_NE(iref, nullptr) << "Failed to cast to IREF";
  EXPECT_EQ(iref->GetBoxes().size(), 3u);
  auto dimg = iref->GetTypedBox<ISOBMFF::DIMG>("dimg");
  ASSERT_NE(dimg, nullptr);
  EXPECT_EQ(dimg->GetFromItemID(), 49u);
  EXPECT_THAT(dimg->GetToItemIDs(), ::testing::ElementsAre(1u, 2u, 3u, 4u));
  auto thmb = iref->GetTypedBox<ISOBMFF::THMB>("thmb");
  ASSERT_NE(thmb, nullptr);
  EXPECT_EQ(thmb->GetFromItemID(), 50u);
  EXPECT_THAT(thmb->GetToItemIDs(), ::testing::ElementsAre(49u));
  auto cdsc = iref->GetTypedBox<ISOBMFF::CDSC>("cdsc");
  ASSERT_NE(cdsc, nullptr);
  EXPECT_EQ(cdsc->GetFromItemID(), 51u);
}

// A synthetic version 1 iref: 32-bit item IDs.
TEST_F(ISOBMFFIREFTest, TestIREFParserVersion1) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // version = 1, flags = 0
      0x01, 0x00, 0x00, 0x00,
      // thmb: from_item_ID = 65536, reference_count = 1, to_item_ID = 49
      0x00, 0x00, 0x00, 0x12, 0x74, 0x68, 0x6d, 0x62, 0x00, 0x01, 0x00, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x00, 0x31};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("iref");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto iref = std::dynamic_pointer_cast<ISOBMFF::IREF>(box);
  ASSERT_NE(iref, nullptr) << "Failed to cast to IREF";
  auto thmb = iref->GetTypedBox<ISOBMFF::THMB>("thmb");
  ASSERT_NE(thmb, nullptr);
  EXPECT_EQ(thmb->GetFromItemID(), 65536u);
  EXPECT_THAT(thmb->GetToItemIDs(), ::testing::ElementsAre(49u));
}
}  // namespace ISOBMFF

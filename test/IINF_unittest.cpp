/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFIINFTest : public ::testing::Test {
 public:
  ISOBMFFIINFTest() {}
  ~ISOBMFFIINFTest() override {}
};

// Built from an Apple iPhone HEIC file (media/IMG1.HEIC): its iinf has 51
// entries; this test keeps 3, with entry_count adjusted.
TEST_F(ISOBMFFIINFTest, TestIINFParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 0, flags = 0, entry_count = 3
      0x00, 0x00, 0x00, 0x00, 0x00, 0x03,
      // infe: item_ID = 1, item_type = 'hvc1'
      0x00, 0x00, 0x00, 0x15, 0x69, 0x6e, 0x66, 0x65,
      0x02, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00,
      0x68, 0x76, 0x63, 0x31, 0x00,
      // infe: item_ID = 49, item_type = 'grid'
      0x00, 0x00, 0x00, 0x15, 0x69, 0x6e, 0x66, 0x65,
      0x02, 0x00, 0x00, 0x00, 0x00, 0x31, 0x00, 0x00,
      0x67, 0x72, 0x69, 0x64, 0x00,
      // infe: item_ID = 51, item_type = 'Exif'
      0x00, 0x00, 0x00, 0x15, 0x69, 0x6e, 0x66, 0x65,
      0x02, 0x00, 0x00, 0x01, 0x00, 0x33, 0x00, 0x00,
      0x45, 0x78, 0x69, 0x66, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("iinf");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto iinf = std::dynamic_pointer_cast<ISOBMFF::IINF>(box);
  ASSERT_NE(iinf, nullptr) << "Failed to cast to IINF";
  EXPECT_EQ(iinf->GetEntries().size(), 3u);
  ASSERT_NE(iinf->GetItemInfo(49), nullptr);
  EXPECT_EQ(iinf->GetItemInfo(49)->GetItemType(), "grid");
  ASSERT_NE(iinf->GetItemInfo(51), nullptr);
  EXPECT_EQ(iinf->GetItemInfo(51)->GetItemType(), "Exif");
  EXPECT_EQ(iinf->GetItemInfo(2), nullptr);
}

// A truncated iinf (1 of the 2 entry_count bytes).
TEST_F(ISOBMFFIINFTest, TestIINFParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 0, flags = 0, entry_count truncated
      0x00, 0x00, 0x00, 0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("iinf");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

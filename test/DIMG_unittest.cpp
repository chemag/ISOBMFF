/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFDIMGTest : public ::testing::Test {
 public:
  ISOBMFFDIMGTest() {}
  ~ISOBMFFDIMGTest() override {}
};

// following example input is from an Apple iPhone HEIC file (media/IMG1.HEIC)
// (cut to 4 of 48 references). Parsed inside a version 0 iref, as in the file.
TEST_F(ISOBMFFDIMGTest, TestDIMGParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // dimg box size: 20 bytes, box header not included
      // from_item_ID = 49, reference_count, to_item_IDs
      0x00, 0x31, 0x00, 0x04, 0x00, 0x01, 0x00, 0x02,
      0x00, 0x03, 0x00, 0x04
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  // the item IDs are 16-bit or 32-bit depending on the parent iref version
  ISOBMFF::IREF iref;
  parser.SetInfo("iref", &iref);
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("dimg");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  parser.SetInfo("iref", nullptr);
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto dimg = std::dynamic_pointer_cast<ISOBMFF::DIMG>(box);
  ASSERT_NE(dimg, nullptr) << "Failed to cast to DIMG";
  EXPECT_EQ(dimg->GetFromItemID(), 49u);
  EXPECT_THAT(dimg->GetToItemIDs(), ::testing::ElementsAre(1u, 2u, 3u, 4u));
}

// A synthetic dimg inside a version 1 iref: 32-bit item IDs.
TEST_F(ISOBMFFDIMGTest, TestDIMGParserVersion1) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // from_item_ID = 65536, reference_count = 2, to_item_ID = 1, 2
      0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x00, 0x02
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  // the item IDs are 16-bit or 32-bit depending on the parent iref version
  ISOBMFF::IREF iref;
  iref.SetVersion(1);
  parser.SetInfo("iref", &iref);
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("dimg");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  parser.SetInfo("iref", nullptr);
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto dimg = std::dynamic_pointer_cast<ISOBMFF::DIMG>(box);
  ASSERT_NE(dimg, nullptr) << "Failed to cast to DIMG";
  EXPECT_EQ(dimg->GetFromItemID(), 65536u);
  EXPECT_THAT(dimg->GetToItemIDs(), ::testing::ElementsAre(1u, 2u));
}

// A dimg declaring 4 references but carrying 2.
TEST_F(ISOBMFFDIMGTest, TestDIMGParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // from_item_ID = 49, reference_count = 4, to_item_ID = 1, 2
      0x00, 0x31, 0x00, 0x04, 0x00, 0x01, 0x00, 0x02
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  // the item IDs are 16-bit or 32-bit depending on the parent iref version
  ISOBMFF::IREF iref;
  parser.SetInfo("iref", &iref);
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("dimg");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  parser.SetInfo("iref", nullptr);
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

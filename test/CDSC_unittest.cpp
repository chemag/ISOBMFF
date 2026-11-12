/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFCDSCTest : public ::testing::Test {
 public:
  ISOBMFFCDSCTest() {}
  ~ISOBMFFCDSCTest() override {}
};

// following example input is from an Apple iPhone HEIC file (media/IMG1.HEIC).
// Parsed inside a version 0 iref, as in the file.
TEST_F(ISOBMFFCDSCTest, TestCDSCParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // cdsc box size: 14 bytes, box header not included
      // from_item_ID = 51, reference_count, to_item_IDs
      0x00, 0x33, 0x00, 0x01, 0x00, 0x31
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  // the item IDs are 16-bit or 32-bit depending on the parent iref version
  ISOBMFF::IREF iref;
  parser.SetInfo("iref", &iref);
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("cdsc");

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

  auto cdsc = std::dynamic_pointer_cast<ISOBMFF::CDSC>(box);
  ASSERT_NE(cdsc, nullptr) << "Failed to cast to CDSC";
  EXPECT_EQ(cdsc->GetFromItemID(), 51u);
  EXPECT_THAT(cdsc->GetToItemIDs(), ::testing::ElementsAre(49u));
}
}  // namespace ISOBMFF

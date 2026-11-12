/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFURNTest : public ::testing::Test {
 public:
  ISOBMFFURNTest() {}
  ~ISOBMFFURNTest() override {}
};

// A synthetic urn box (DataEntryUrnBox) with a name and no location (the
// location is optional in a urn entry).
TEST_F(ISOBMFFURNTest, TestURNParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 0, flags = 1 (media data in the same file)
      0x00, 0x00, 0x00, 0x01,
      // name = "urn:example"
      0x75, 0x72, 0x6e, 0x3a, 0x65, 0x78, 0x61, 0x6d,
      0x70, 0x6c, 0x65, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("urn ");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto urn = std::dynamic_pointer_cast<ISOBMFF::URN>(box);
  ASSERT_NE(urn, nullptr) << "Failed to cast to URN";
  EXPECT_EQ(urn->GetVersion(), 0);
  EXPECT_EQ(urn->GetFlags(), 1u);
  EXPECT_EQ(urn->GetURNName(), "urn:example");
  // the box name is still the box type
  EXPECT_EQ(urn->GetName(), "urn ");
  EXPECT_EQ(urn->GetLocation(), "");
}

// A synthetic urn box with a name and a location. The location runs into
// the end of the box without its NUL terminator, which some writers leave
// out: it is kept.
TEST_F(ISOBMFFURNTest, TestURNParserNameAndLocation) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 0, flags = 0 (media data in another file)
      0x00, 0x00, 0x00, 0x00,
      // name = "urn:x"
      0x75, 0x72, 0x6e, 0x3a, 0x78, 0x00,
      // location = "a.mp4" (no NUL terminator)
      0x61, 0x2e, 0x6d, 0x70, 0x34
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("urn ");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto urn = std::dynamic_pointer_cast<ISOBMFF::URN>(box);
  ASSERT_NE(urn, nullptr) << "Failed to cast to URN";
  EXPECT_EQ(urn->GetFlags(), 0u);
  EXPECT_EQ(urn->GetURNName(), "urn:x");
  EXPECT_EQ(urn->GetLocation(), "a.mp4");
}

// A truncated urn (3 of the 4 version and flags bytes).
TEST_F(ISOBMFFURNTest, TestURNParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version and flags truncated
      0x00, 0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("urn ");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

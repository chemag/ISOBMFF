/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFINFETest : public ::testing::Test {
 public:
  ISOBMFFINFETest() {}
  ~ISOBMFFINFETest() override {}
};

// following example input is from an Apple iPhone HEIC file (media/IMG1.HEIC).
TEST_F(ISOBMFFINFETest, TestINFEParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // infe box size: 21 bytes, box header not included
      // version = 2, flags = 1 (hidden)
      0x02, 0x00, 0x00, 0x01,
      // item_ID = 1, item_protection_index = 0
      0x00, 0x01, 0x00, 0x00,
      // item_type = 'hvc1'
      0x68, 0x76, 0x63, 0x31,
      // item_name = ""
      0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("infe");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto infe = std::dynamic_pointer_cast<ISOBMFF::INFE>(box);
  ASSERT_NE(infe, nullptr) << "Failed to cast to INFE";
  EXPECT_EQ(infe->GetVersion(), 2);
  EXPECT_EQ(infe->GetFlags(), 1u);
  EXPECT_EQ(infe->GetItemID(), 1u);
  EXPECT_EQ(infe->GetItemProtectionIndex(), 0);
  EXPECT_EQ(infe->GetItemType(), "hvc1");
  EXPECT_EQ(infe->GetItemName(), "");
}

// A synthetic version 2 mime item: item_name comes before content_type.
TEST_F(ISOBMFFINFETest, TestINFEParserMime) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 2, flags = 0
      0x02, 0x00, 0x00, 0x00,
      // item_ID = 64, item_protection_index = 0
      0x00, 0x40, 0x00, 0x00,
      // item_type = 'mime'
      0x6d, 0x69, 0x6d, 0x65,
      // item_name = "XMP"
      0x58, 0x4d, 0x50, 0x00,
      // content_type = "application/rdf+xml"
      0x61, 0x70, 0x70, 0x6c, 0x69, 0x63, 0x61, 0x74,
      0x69, 0x6f, 0x6e, 0x2f, 0x72, 0x64, 0x66, 0x2b,
      0x78, 0x6d, 0x6c, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("infe");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto infe = std::dynamic_pointer_cast<ISOBMFF::INFE>(box);
  ASSERT_NE(infe, nullptr) << "Failed to cast to INFE";
  EXPECT_EQ(infe->GetItemID(), 64u);
  EXPECT_EQ(infe->GetItemType(), "mime");
  EXPECT_EQ(infe->GetItemName(), "XMP");
  EXPECT_EQ(infe->GetContentType(), "application/rdf+xml");
  EXPECT_EQ(infe->GetContentEncoding(), "");
}

// A synthetic version 2 mime item with the optional content_encoding.
TEST_F(ISOBMFFINFETest, TestINFEParserMimeContentEncoding) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 2, flags = 0
      0x02, 0x00, 0x00, 0x00,
      // item_ID = 65, item_protection_index = 0
      0x00, 0x41, 0x00, 0x00,
      // item_type = 'mime'
      0x6d, 0x69, 0x6d, 0x65,
      // item_name = ""
      0x00,
      // content_type = "text/plain"
      0x74, 0x65, 0x78, 0x74, 0x2f, 0x70, 0x6c, 0x61,
      0x69, 0x6e, 0x00,
      // content_encoding = "gzip"
      0x67, 0x7a, 0x69, 0x70, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("infe");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto infe = std::dynamic_pointer_cast<ISOBMFF::INFE>(box);
  ASSERT_NE(infe, nullptr) << "Failed to cast to INFE";
  EXPECT_EQ(infe->GetItemName(), "");
  EXPECT_EQ(infe->GetContentType(), "text/plain");
  EXPECT_EQ(infe->GetContentEncoding(), "gzip");
}

// A synthetic version 3 infe, with a 32-bit item_ID.
TEST_F(ISOBMFFINFETest, TestINFEParserVersion3) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 3, flags = 0
      0x03, 0x00, 0x00, 0x00,
      // item_ID = 65536
      0x00, 0x01, 0x00, 0x00,
      // item_protection_index = 0
      0x00, 0x00,
      // item_type = 'Exif'
      0x45, 0x78, 0x69, 0x66,
      // item_name = ""
      0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("infe");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto infe = std::dynamic_pointer_cast<ISOBMFF::INFE>(box);
  ASSERT_NE(infe, nullptr) << "Failed to cast to INFE";
  EXPECT_EQ(infe->GetVersion(), 3);
  EXPECT_EQ(infe->GetItemID(), 65536u);
  EXPECT_EQ(infe->GetItemType(), "Exif");
}

// A synthetic version 2 infe that ends right after item_type (no item_name).
// Tolerated, as before item_name was parsed.
TEST_F(ISOBMFFINFETest, TestINFEParserNoItemName) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 2, flags = 0
      0x02, 0x00, 0x00, 0x00,
      // item_ID = 5, item_protection_index = 0
      0x00, 0x05, 0x00, 0x00,
      // item_type = 'hvc1'
      0x68, 0x76, 0x63, 0x31
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("infe");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto infe = std::dynamic_pointer_cast<ISOBMFF::INFE>(box);
  ASSERT_NE(infe, nullptr) << "Failed to cast to INFE";
  EXPECT_EQ(infe->GetItemID(), 5u);
  EXPECT_EQ(infe->GetItemType(), "hvc1");
  EXPECT_EQ(infe->GetItemName(), "");
}

// A truncated infe (1 of the 2 item_ID bytes).
TEST_F(ISOBMFFINFETest, TestINFEParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 2, flags = 0, item_ID truncated
      0x02, 0x00, 0x00, 0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("infe");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

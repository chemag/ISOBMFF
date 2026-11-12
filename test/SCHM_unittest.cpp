/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFSCHMTest : public ::testing::Test {
 public:
  ISOBMFFSCHMTest() {}
  ~ISOBMFFSCHMTest() override {}
};

// A synthetic schm, as in the sinf of a CENC encrypted track.
TEST_F(ISOBMFFSCHMTest, TestSCHMParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 0, flags = 0
      0x00, 0x00, 0x00, 0x00,
      // scheme_type = 'cenc', scheme_version = 0x00010000
      0x63, 0x65, 0x6e, 0x63, 0x00, 0x01, 0x00, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("schm");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto schm = std::dynamic_pointer_cast<ISOBMFF::SCHM>(box);
  ASSERT_NE(schm, nullptr) << "Failed to cast to SCHM";
  EXPECT_EQ(schm->GetSchemeType(), "cenc");
  EXPECT_EQ(schm->GetSchemeVersion(), 0x00010000u);
  EXPECT_EQ(schm->GetSchemeURI(), "");
}

// A synthetic schm with flags & 1 set: a scheme_uri follows.
TEST_F(ISOBMFFSCHMTest, TestSCHMParserURI) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 0, flags = 1
      0x00, 0x00, 0x00, 0x01,
      // scheme_type = 'cbcs', scheme_version = 0x00010000
      0x63, 0x62, 0x63, 0x73, 0x00, 0x01, 0x00, 0x00,
      // scheme_uri = "urn:example:scheme"
      0x75, 0x72, 0x6e, 0x3a, 0x65, 0x78, 0x61, 0x6d,
      0x70, 0x6c, 0x65, 0x3a, 0x73, 0x63, 0x68, 0x65,
      0x6d, 0x65, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("schm");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto schm = std::dynamic_pointer_cast<ISOBMFF::SCHM>(box);
  ASSERT_NE(schm, nullptr) << "Failed to cast to SCHM";
  EXPECT_EQ(schm->GetSchemeType(), "cbcs");
  EXPECT_EQ(schm->GetSchemeURI(), "urn:example:scheme");
}

// A truncated schm (no scheme_version).
TEST_F(ISOBMFFSCHMTest, TestSCHMParserTruncated) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // version = 0, flags = 0, scheme_type = 'cenc'
      0x00, 0x00, 0x00, 0x00, 0x63, 0x65, 0x6e, 0x63
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("schm");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

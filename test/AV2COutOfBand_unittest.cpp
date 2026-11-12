/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFAV2COutOfBandTest : public ::testing::Test {
 public:
  ISOBMFFAV2COutOfBandTest() {}
  ~ISOBMFFAV2COutOfBandTest() override {}
};

// A synthetic av2C (no AV2 encoder output available yet) carrying 2 OBUs.
// The OBU payloads are arbitrary: the box only frames them.
TEST_F(ISOBMFFAV2COutOfBandTest, TestAV2COutOfBandParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // av2C box size: 19 bytes, box header not included
      // reserved1 = 0
      0x00,
      // config_obus_count_minus1 = 1
      0x01,
      // config_obu[0]: leb128 length = 3, then 3 bytes
      0x03, 0x08, 0x00, 0x2a,
      // config_obu[1]: leb128 length = 2 (non-minimal 3-byte encoding),
      // then 2 bytes
      0x82, 0x80, 0x00, 0x0c, 0x01
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("av2C");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto av2c = std::dynamic_pointer_cast<ISOBMFF::AV2COutOfBand>(box);
  ASSERT_NE(av2c, nullptr) << "Failed to cast to AV2COutOfBand";

  EXPECT_EQ(av2c->GetName(), "av2C");
  EXPECT_EQ(av2c->GetReserved1(), 0);
  EXPECT_EQ(av2c->GetConfigOBUsCountMinus1(), 1);
  auto config_obus = av2c->GetConfigOBUs();
  ASSERT_EQ(config_obus.size(), 2);
  EXPECT_THAT(config_obus[0], ::testing::ElementsAreArray({0x08, 0x00, 0x2a}));
  EXPECT_THAT(config_obus[1], ::testing::ElementsAreArray({0x0c, 0x01}));
}

// An OBU whose leb128 length runs past the end of the box.
TEST_F(ISOBMFFAV2COutOfBandTest, TestAV2COutOfBandParserTruncatedOBU) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // reserved1 = 0, config_obus_count_minus1 = 0
      0x00, 0x00,
      // config_obu[0]: leb128 length = 16, but only 2 bytes follow
      0x10, 0x08, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("av2C");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}

// A leb128 length longer than the 8 bytes leb128() allows.
TEST_F(ISOBMFFAV2COutOfBandTest, TestAV2COutOfBandParserOverlongLeb128) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // reserved1 = 0, config_obus_count_minus1 = 0
      0x00, 0x00,
      // config_obu[0]: 9 leb128 bytes, all with the continuation bit set
      0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80,
      0x01
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("av2C");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_TRUE(error);
}
}  // namespace ISOBMFF

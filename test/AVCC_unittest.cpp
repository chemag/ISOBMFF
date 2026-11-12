/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFAVCCTest : public ::testing::Test {
 public:
  ISOBMFFAVCCTest() {}
  ~ISOBMFFAVCCTest() override {}
};

TEST_F(ISOBMFFAVCCTest, TestAVCCParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // following example input is from a sample .mp4 video file
      // avcC size: 57 bytes
      // 0x00, 0x00, 0x00, 0x39,
      // avcC
      // 0x61, 0x76, 0x63, 0x43,
      // avcC content:
      0x01, 0x64, 0x00, 0x34, 0xff, 0xe1, 0x00, 0x1c,
      0x67, 0x64, 0x00, 0x34, 0xac, 0xd9, 0x40, 0x6c,
      0x01, 0x21, 0xa6, 0xa1, 0x22, 0x41, 0x28, 0x00,
      0x00, 0x03, 0x00, 0x08, 0x00, 0x00, 0x07, 0x80,
      0x78, 0xc1, 0x8c, 0xb0, 0x01, 0x00, 0x06, 0x68,
      0xeb, 0xe3, 0xcb, 0x22, 0xc0, 0xfd, 0xf8, 0xf8,
      0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("avcC");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  // Validate AVCC box
  auto avcc = std::dynamic_pointer_cast<ISOBMFF::AVCC>(box);
  ASSERT_NE(avcc, nullptr) << "Failed to cast to AVCC";

  // Validate configuration version
  EXPECT_EQ(avcc->GetConfigurationVersion(), 1);

  // Validate AVC profile indication
  EXPECT_EQ(avcc->GetAVCProfileIndication(), 0x64);

  // Validate profile compatibility
  EXPECT_EQ(avcc->GetProfileCompatibility(), 0x00);

  // Validate AVC level indication
  EXPECT_EQ(avcc->GetAVCLevelIndication(), 52);

  // Validate length size minus one
  EXPECT_EQ(avcc->GetLengthSizeMinusOne(), 3);

  // Validate number of sequence parameter sets
  EXPECT_EQ(avcc->GetNumOfSequenceParameterSets(), 1);

  // Validate number of picture parameter sets
  EXPECT_EQ(avcc->GetNumOfPictureParameterSets(), 1);

  // Validate SPS NAL units
  EXPECT_EQ(avcc->GetSequenceParameterSetNALUnits().size(), 1);
  EXPECT_EQ(avcc->GetSequenceParameterSetNALUnits()[0]->GetData().size(), 28);

  // Validate PPS NAL units
  EXPECT_EQ(avcc->GetPictureParameterSetNALUnits().size(), 1);
  EXPECT_EQ(avcc->GetPictureParameterSetNALUnits()[0]->GetData().size(), 6);

  // Validate the High profile fields (0xfd 0xf8 0xf8 0x00): 4:2:0, 8-bit
  // luma and chroma, no SPS extensions
  EXPECT_TRUE(avcc->HasHighProfileFields());
  EXPECT_EQ(avcc->GetChromaFormat(), 1);
  EXPECT_EQ(avcc->GetBitDepthLumaMinus8(), 0);
  EXPECT_EQ(avcc->GetBitDepthChromaMinus8(), 0);
  EXPECT_EQ(avcc->GetNumOfSequenceParameterSetExt(), 0);
  EXPECT_EQ(avcc->GetSequenceParameterSetExtNALUnits().size(), 0);
}
// A synthetic avcC whose only SPS declares 256 bytes, but only 6 are left.
// The SPS used to be kept as a 256-byte zero-filled buffer.
TEST_F(ISOBMFFAVCCTest, TestAVCCTruncatedNALUnit) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // configurationVersion, profile, compatibility, level
      0x01, 0x64, 0x00, 0x1f,
      // lengthSizeMinusOne: 3
      0xff,
      // numOfSequenceParameterSets: 1
      0xe1,
      // SPS: length 256, truncated (6 bytes left)
      0x01, 0x00, 0x67, 0x64, 0x00, 0x1f, 0xac, 0xd9
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("avcC");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  auto avcc = std::dynamic_pointer_cast<ISOBMFF::AVCC>(box);
  ASSERT_NE(avcc, nullptr) << "Failed to cast to AVCC";
  // no NAL unit with fabricated (zero-filled) data
  EXPECT_EQ(avcc->GetSequenceParameterSetNALUnits().size(), 0);
  for (const auto& nal_unit : avcc->GetPictureParameterSetNALUnits()) {
    std::vector<uint8_t> data = nal_unit->GetData();
    for (const auto& byte : data) {
      EXPECT_NE(byte, 0) << "zero-filled NAL unit data";
    }
    EXPECT_LE(data.size(), buffer.size());
  }
}

// A High 4:2:2 profile (122) avcC with 10-bit luma and chroma and one SPS
// extension NAL unit (the SPS and PPS are shortened).
TEST_F(ISOBMFFAVCCTest, TestAVCCHigh422) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // configurationVersion, profile (122), compatibility, level
      0x01, 0x7a, 0x00, 0x28,
      // lengthSizeMinusOne: 3
      0xff,
      // numOfSequenceParameterSets: 1
      0xe1,
      // SPS: length 4
      0x00, 0x04, 0x67, 0x7a, 0x00, 0x28,
      // numOfPictureParameterSets: 1
      0x01,
      // PPS: length 2
      0x00, 0x02, 0x68, 0xee,
      // chroma_format: 2 (4:2:2)
      0xfe,
      // bit_depth_luma_minus8: 2
      0xfa,
      // bit_depth_chroma_minus8: 2
      0xfa,
      // numOfSequenceParameterSetExt: 1
      0x01,
      // SPS extension: length 3
      0x00, 0x03, 0x6d, 0x01, 0x02
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("avcC");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error) << error.GetMessage();
  auto avcc = std::dynamic_pointer_cast<ISOBMFF::AVCC>(box);
  ASSERT_NE(avcc, nullptr) << "Failed to cast to AVCC";
  EXPECT_EQ(avcc->GetAVCProfileIndication(), 122);
  EXPECT_TRUE(avcc->HasHighProfileFields());
  EXPECT_EQ(avcc->GetChromaFormat(), 2);
  EXPECT_EQ(avcc->GetBitDepthLumaMinus8(), 2);
  EXPECT_EQ(avcc->GetBitDepthChromaMinus8(), 2);
  EXPECT_EQ(avcc->GetNumOfSequenceParameterSetExt(), 1);
  ASSERT_EQ(avcc->GetSequenceParameterSetExtNALUnits().size(), 1);
  EXPECT_EQ(avcc->GetSequenceParameterSetExtNALUnits()[0]->GetData(),
            std::vector<uint8_t>({0x6d, 0x01, 0x02}));
}

// A High profile avcC that ends after the PPSs, as some muxers write it: the
// High profile fields are absent, and that is not an error.
TEST_F(ISOBMFFAVCCTest, TestAVCCHighWithoutHighProfileFields) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // configurationVersion, profile (100), compatibility, level
      0x01, 0x64, 0x00, 0x1f,
      // lengthSizeMinusOne: 3
      0xff,
      // numOfSequenceParameterSets: 1
      0xe1,
      // SPS: length 4
      0x00, 0x04, 0x67, 0x64, 0x00, 0x1f,
      // numOfPictureParameterSets: 1
      0x01,
      // PPS: length 2
      0x00, 0x02, 0x68, 0xee,
      // only 2 of the 4 fixed bytes of the High profile fields
      0xfd, 0xf8
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("avcC");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error) << error.GetMessage();
  auto avcc = std::dynamic_pointer_cast<ISOBMFF::AVCC>(box);
  ASSERT_NE(avcc, nullptr) << "Failed to cast to AVCC";
  EXPECT_EQ(avcc->GetPictureParameterSetNALUnits().size(), 1);
  EXPECT_FALSE(avcc->HasHighProfileFields());
  EXPECT_EQ(avcc->GetChromaFormat(), 0);
  EXPECT_EQ(avcc->GetSequenceParameterSetExtNALUnits().size(), 0);
}

// A Baseline profile (66) avcC has no High profile fields: trailing bytes are
// not read as them.
TEST_F(ISOBMFFAVCCTest, TestAVCCBaselineIgnoresTrailingBytes) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // clang-format off
      // configurationVersion, profile (66), compatibility, level
      0x01, 0x42, 0xc0, 0x1e,
      // lengthSizeMinusOne: 3
      0xff,
      // numOfSequenceParameterSets: 1
      0xe1,
      // SPS: length 4
      0x00, 0x04, 0x67, 0x42, 0xc0, 0x1e,
      // numOfPictureParameterSets: 1
      0x01,
      // PPS: length 2
      0x00, 0x02, 0x68, 0xce,
      // trailing bytes
      0xfd, 0xf8, 0xf8, 0x00
      // clang-format on
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("avcC");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error) << error.GetMessage();
  auto avcc = std::dynamic_pointer_cast<ISOBMFF::AVCC>(box);
  ASSERT_NE(avcc, nullptr) << "Failed to cast to AVCC";
  EXPECT_FALSE(avcc->HasHighProfileFields());
}
}  // namespace ISOBMFF

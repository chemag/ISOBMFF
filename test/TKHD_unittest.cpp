/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <ISOBMFF.hpp>                  // for various
#include <BinaryDataStream.hpp> // for BinaryDataStream
#include <Parser.hpp>           // for Parser

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ISOBMFF {

class ISOBMFFTKHDTest : public ::testing::Test {
public:
  ISOBMFFTKHDTest() {}
  ~ISOBMFFTKHDTest() override {}
};

TEST_F(ISOBMFFTKHDTest, TestTKHDParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t> &buffer = {
      // following example input is from a sample .mp4 video file
      // tkhd size: 92 bytes
      // 0x00, 0x00, 0x00, 0x5c,
      // // tkhd
      // 0x74, 0x6b, 0x68, 0x64,
      // tkhd content
      0x00, 0x00, 0x00, 0x07, 0xe4, 0x00, 0xb3, 0x34,
      0xe4, 0x00, 0xb3, 0x34, 0x00, 0x00, 0x00, 0x01,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x02,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x40, 0x00, 0x00, 0x00, 0x06, 0xc0, 0x00, 0x00,
      0x09, 0x00, 0x00, 0x00
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("tkhd");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  // Validate TKHD box
  auto tkhd = std::dynamic_pointer_cast<ISOBMFF::TKHD>(box);
  EXPECT_TRUE(tkhd != nullptr);

  // Validate TKHD properties
  EXPECT_EQ(tkhd->GetCreationTime(), 3825251124);
  EXPECT_EQ(tkhd->GetModificationTime(), 3825251124);
  EXPECT_EQ(tkhd->GetTrackID(), 1);
  EXPECT_EQ(tkhd->GetDuration(), 0x00003702);
  EXPECT_EQ(tkhd->GetLayer(), 0);
  EXPECT_EQ(tkhd->GetAlternateGroup(), 0);
  EXPECT_EQ(tkhd->GetVolumeRaw(), 0);
  EXPECT_DOUBLE_EQ(tkhd->GetVolume(), 0.0);
  EXPECT_EQ(tkhd->GetWidthRaw(), 1728u << 16);
  EXPECT_DOUBLE_EQ(tkhd->GetWidth(), 1728.0);
  EXPECT_EQ(tkhd->GetHeightRaw(), 2304u << 16);
  EXPECT_DOUBLE_EQ(tkhd->GetHeight(), 2304.0);

  // Validate displayable properties
  auto displayableProperties = tkhd->GetDisplayableProperties();
  ASSERT_EQ(displayableProperties.size(), 15);
  EXPECT_EQ(displayableProperties[0].first, "Version");
  EXPECT_EQ(displayableProperties[1].first, "Flags");
  EXPECT_EQ(displayableProperties[2].first, "Creation time");
  EXPECT_EQ(displayableProperties[3].first, "Modification time");
  EXPECT_EQ(displayableProperties[4].first, "Track ID");
  EXPECT_EQ(displayableProperties[5].first, "Duration");
  EXPECT_EQ(displayableProperties[6].first, "Layer");
  EXPECT_EQ(displayableProperties[7].first, "Alternate group");
  EXPECT_EQ(displayableProperties[8].first, "Volume (raw)");
  EXPECT_EQ(displayableProperties[9].first, "Volume");
  EXPECT_EQ(displayableProperties[10].first, "Matrix");
  EXPECT_EQ(displayableProperties[11].first, "Width (raw)");
  EXPECT_EQ(displayableProperties[11].second, std::to_string(1728u << 16));
  EXPECT_EQ(displayableProperties[12].first, "Width");
  EXPECT_EQ(displayableProperties[12].second, std::to_string(1728.0));
  EXPECT_EQ(displayableProperties[13].first, "Height (raw)");
  EXPECT_EQ(displayableProperties[14].first, "Height");
}

// width and height are unsigned 16.16 fixed-point fields: a fractional size
// keeps its fraction, and negative values saturate to 0
TEST_F(ISOBMFFTKHDTest, TestTKHDFixedPoint) {
  ISOBMFF::TKHD tkhd;

  tkhd.SetWidthRaw(0x02d08000);  // 720.5
  EXPECT_DOUBLE_EQ(tkhd.GetWidth(), 720.5);
  tkhd.SetHeight(480.25);
  EXPECT_EQ(tkhd.GetHeightRaw(), 0x01e04000u);
  tkhd.SetWidth(-1.0);
  EXPECT_EQ(tkhd.GetWidthRaw(), 0u);

  tkhd.SetVolumeRaw(0x0100);
  EXPECT_DOUBLE_EQ(tkhd.GetVolume(), 1.0);
  tkhd.SetVolume(0.5);
  EXPECT_EQ(tkhd.GetVolumeRaw(), 0x0080);
}
} // namespace ISOBMFF

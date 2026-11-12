/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <ISOBMFF.hpp>                  // for various
#include <BinaryDataStream.hpp> // for BinaryDataStream
#include <Parser.hpp>           // for Parser

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ISOBMFF {

class ISOBMFFMVHDTest : public ::testing::Test {
public:
  ISOBMFFMVHDTest() {}
  ~ISOBMFFMVHDTest() override {}
};

TEST_F(ISOBMFFMVHDTest, TestMVHDParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t> &buffer = {
      // following example input is from a sample .mp4 video file
      // mvhd size: 108 bytes
      // 0x00, 0x00, 0x00, 0x6c,
      // // mvdh
      // 0x66, 0x74, 0x79, 0x70,
      // mvhd content
      0x00, 0x00, 0x00, 0x00, 0xe4, 0x00, 0xb3, 0x34, 0xe4, 0x00, 0xb3, 0x34,
      0x00, 0x00, 0x27, 0x10, 0x00, 0x00, 0x37, 0x02, 0x00, 0x01, 0x00, 0x00,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x03
  };

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("mvhd");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  // Validate MVHD box
  auto mvhd = std::dynamic_pointer_cast<ISOBMFF::MVHD>(box);
  EXPECT_TRUE(mvhd != nullptr);

  // Validate displayable properties
  EXPECT_EQ(mvhd->GetCreationTime(), 0xe400b334);
  EXPECT_EQ(mvhd->GetModificationTime(), 0xe400b334);
  EXPECT_EQ(mvhd->GetTimescale(), 10000);
  EXPECT_EQ(mvhd->GetDuration(), 14082);
  EXPECT_EQ(mvhd->GetRateRaw(), 0x00010000);
  EXPECT_DOUBLE_EQ(mvhd->GetRate(), 1.0);
  EXPECT_EQ(mvhd->GetVolumeRaw(), 0x0100);
  EXPECT_DOUBLE_EQ(mvhd->GetVolume(), 1.0);
  EXPECT_EQ(mvhd->GetNextTrackID(), 3);
  auto displayableProperties = mvhd->GetDisplayableProperties();

  // Adjust the expected size based on actual properties
  ASSERT_EQ(displayableProperties.size(), 12);

  // Validate specific properties
  EXPECT_EQ(displayableProperties[0].first, "Version");
  EXPECT_EQ(displayableProperties[1].first, "Flags");
  EXPECT_EQ(displayableProperties[2].first, "Creation time");
  EXPECT_EQ(displayableProperties[2].second, std::to_string(0xe400b334));
  EXPECT_EQ(displayableProperties[3].first, "Modification time");
  EXPECT_EQ(displayableProperties[3].second, std::to_string(0xe400b334));
  EXPECT_EQ(displayableProperties[4].first, "Timescale");
  EXPECT_EQ(displayableProperties[4].second, std::to_string(10000));
  EXPECT_EQ(displayableProperties[5].first, "Duration");
  EXPECT_EQ(displayableProperties[5].second, std::to_string(14082));
  EXPECT_EQ(displayableProperties[6].first, "Rate (raw)");
  EXPECT_EQ(displayableProperties[6].second, std::to_string(0x00010000));
  EXPECT_EQ(displayableProperties[7].first, "Rate");
  EXPECT_EQ(displayableProperties[7].second, std::to_string(1.0));
  EXPECT_EQ(displayableProperties[8].first, "Volume (raw)");
  EXPECT_EQ(displayableProperties[8].second, std::to_string(0x0100));
  EXPECT_EQ(displayableProperties[9].first, "Volume");
  EXPECT_EQ(displayableProperties[9].second, std::to_string(1.0));
  EXPECT_EQ(displayableProperties[10].first, "Matrix");
  EXPECT_EQ(displayableProperties[10].second, mvhd->GetMatrix().ToString());
  EXPECT_EQ(displayableProperties[11].first, "Next track ID");
  EXPECT_EQ(displayableProperties[11].second, std::to_string(3));
}

// rate and volume are signed fixed-point fields (16.16 and 8.8): the raw
// setters keep the stored value, and the derived setters round to the nearest
// representable value and saturate to the field range
TEST_F(ISOBMFFMVHDTest, TestMVHDFixedPoint) {
  ISOBMFF::MVHD mvhd;

  mvhd.SetRateRaw(static_cast<int32_t>(0xffff8000));  // -0.5
  EXPECT_DOUBLE_EQ(mvhd.GetRate(), -0.5);
  mvhd.SetRate(2.25);
  EXPECT_EQ(mvhd.GetRateRaw(), 0x00024000);
  mvhd.SetRate(-1.0);
  EXPECT_EQ(mvhd.GetRateRaw(), static_cast<int32_t>(0xffff0000));
  mvhd.SetRate(1e9);  // saturates
  EXPECT_EQ(mvhd.GetRateRaw(), INT32_MAX);

  mvhd.SetVolumeRaw(0x0080);  // 0.5
  EXPECT_DOUBLE_EQ(mvhd.GetVolume(), 0.5);
  mvhd.SetVolume(0.75);
  EXPECT_EQ(mvhd.GetVolumeRaw(), 0x00c0);
  mvhd.SetVolume(-0.25);
  EXPECT_EQ(mvhd.GetVolumeRaw(), -0x40);
  // 1/1024 is not representable in 8.8: rounds to the nearest 1/256
  mvhd.SetVolume(1.0 + 3.0 / 1024.0);
  EXPECT_EQ(mvhd.GetVolumeRaw(), 0x0101);
}
} // namespace ISOBMFF

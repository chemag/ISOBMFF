/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <BinaryDataStream.hpp>  // for BinaryDataStream
#include <ISOBMFF.hpp>           // for various
#include <Parser.hpp>            // for Parser

namespace ISOBMFF {

class ISOBMFFIPCOTest : public ::testing::Test {
 public:
  ISOBMFFIPCOTest() {}
  ~ISOBMFFIPCOTest() override {}
};

// Built from item properties in an Apple iPhone HEIC file (media/IMG1.HEIC)
// (ispe, irot) and media/IMG2.HEIC (pixi).
TEST_F(ISOBMFFIPCOTest, TestIPCOParser) {
  // fuzzer::conv: data
  const std::vector<uint8_t>& buffer = {
      // ispe: 512x512
      0x00, 0x00, 0x00, 0x14, 0x69, 0x73, 0x70, 0x65, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00,
      // irot: angle = 3
      0x00, 0x00, 0x00, 0x09, 0x69, 0x72, 0x6f, 0x74, 0x03,
      // pixi: 3 channels, 8 bits each
      0x00, 0x00, 0x00, 0x10, 0x70, 0x69, 0x78, 0x69, 0x00, 0x00, 0x00, 0x00,
      0x03, 0x08, 0x08, 0x08};

  // fuzzer::conv: begin
  ISOBMFF::BinaryDataStream stream(buffer);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("ipco");

  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  // fuzzer::conv: end

  EXPECT_FALSE(error);

  auto ipco = std::dynamic_pointer_cast<ISOBMFF::IPCO>(box);
  ASSERT_NE(ipco, nullptr) << "Failed to cast to IPCO";
  auto ispe =
      std::dynamic_pointer_cast<ISOBMFF::ISPE>(ipco->GetPropertyAtIndex(0));
  ASSERT_NE(ispe, nullptr);
  EXPECT_EQ(ispe->GetImageWidth(), 512u);
  auto irot =
      std::dynamic_pointer_cast<ISOBMFF::IROT>(ipco->GetPropertyAtIndex(1));
  ASSERT_NE(irot, nullptr);
  EXPECT_EQ(irot->GetAngle(), 3);
  EXPECT_NE(
      std::dynamic_pointer_cast<ISOBMFF::PIXI>(ipco->GetPropertyAtIndex(2)),
      nullptr);
  EXPECT_EQ(ipco->GetPropertyAtIndex(3), nullptr);
}
}  // namespace ISOBMFF

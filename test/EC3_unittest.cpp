/*
 * Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gtest/gtest.h>

#include <ISOBMFF.hpp>
#include <utility>

namespace ISOBMFF {
namespace {
std::vector<uint8_t> SampleEntry() {
  return {// EC3SampleEntry fields, excluding box header.
          0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 16, 0, 0, 0,
          0, 0xbb, 0x80, 0, 0,
          // Exact Android Atmos dec3 and an optional btrt child.
          0, 0, 0, 15, 'd', 'e', 'c', '3', 0x18, 0, 0x20, 0x0f, 0, 1, 12, 0, 0,
          0, 20, 'b', 't', 'r', 't', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}
Error ReadEntry(EC3& box, const std::vector<uint8_t>& data) {
  BinaryDataStream stream(data);
  Parser parser;
  return box.ReadData(parser, stream);
}
}  // namespace
TEST(ISOBMFFEC3Test, ConfigurationAndOptionalChildRetained) {
  EC3 box;
  ASSERT_FALSE(ReadEntry(box, SampleEntry()));
  EXPECT_EQ("ec-3", box.GetName());
  EXPECT_EQ(1, box.GetDataReferenceIndex());
  EXPECT_EQ(2, box.GetChannelCount());  // Placeholder: real service is 5.1.
  EXPECT_EQ(16, box.GetSampleSize());
  EXPECT_EQ(48000u << 16, box.GetSampleRateRaw());
  auto dec3 = box.GetTypedBox<DEC3>("dec3");
  ASSERT_NE(nullptr, dec3);
  EXPECT_EQ(6, dec3->GetIndependentSubstreams()[0].GetChannelCount());
  EXPECT_TRUE(dec3->GetFlagEC3ExtensionTypeA());
  EXPECT_NE(nullptr, box.GetBox("btrt"));
  EC3 copy(box);
  EC3 move(std::move(copy));
  EC3 assign;
  assign = move;
  ASSERT_NE(nullptr, assign.GetTypedBox<DEC3>("dec3"));
  EXPECT_NE(nullptr, assign.GetBox("btrt"));
  EXPECT_EQ(box.GetSampleRateRaw(), assign.GetSampleRateRaw());
  EXPECT_FALSE(assign.GetDisplayableProperties().empty());
}
TEST(ISOBMFFEC3Test, TruncatedHeaderAndConfiguration) {
  auto data = SampleEntry();
  for (size_t size = 0; size < 43; ++size) {
    EC3 box;
    auto prefix = data;
    prefix.resize(size);
    EXPECT_TRUE(ReadEntry(box, prefix));
  }
  // Size-consistent child with a truncated JOC extension, not just a short
  // file.
  data.resize(42);
  data[31] = 14;
  EC3 box;
  EXPECT_TRUE(ReadEntry(box, data));
}
TEST(ISOBMFFEC3Test, MissingDuplicateAndMalformedChildRejected) {
  auto data = SampleEntry();
  EC3 box;
  auto missing = data;
  missing.erase(missing.begin() + 28, missing.begin() + 43);
  EXPECT_TRUE(ReadEntry(box, missing));
  auto duplicate = data;
  duplicate.insert(duplicate.end(), data.begin() + 28, data.begin() + 43);
  EXPECT_TRUE(ReadEntry(box, duplicate));
  auto malformed = data;
  malformed.pop_back();  // Optional child cannot escape its declared size.
  EXPECT_TRUE(ReadEntry(box, malformed));
}
TEST(ISOBMFFEC3Test, ExactFourCCRegistrationAndNestedParsing) {
  Parser parser;
  EXPECT_NE(nullptr, std::dynamic_pointer_cast<EC3>(parser.CreateBox("ec-3")));
  EXPECT_EQ(nullptr, std::dynamic_pointer_cast<EC3>(parser.CreateBox("EC-3")));
  EXPECT_EQ(nullptr, std::dynamic_pointer_cast<DEC3>(parser.CreateBox("DEC3")));
  auto data = SampleEntry();
  std::vector<uint8_t> file = {
      0, 0, 0, 8, 'f', 'r', 'e', 'e', 0, 0, 0, 87, 's', 't', 's', 'd',
      0, 0, 0, 0, 0,   0,   0,   1,   0, 0, 0, 71, 'e', 'c', '-', '3'};
  file.insert(file.end(), data.begin(), data.end());
  ASSERT_FALSE(parser.Parse(file));
  auto stsd = parser.GetFile()->GetTypedBox<STSD>("stsd");
  ASSERT_NE(nullptr, stsd);
  auto ec3 = stsd->GetTypedBox<EC3>("ec-3");
  ASSERT_NE(nullptr, ec3);
  EXPECT_NE(nullptr, ec3->GetTypedBox<DEC3>("dec3"));
}
}  // namespace ISOBMFF

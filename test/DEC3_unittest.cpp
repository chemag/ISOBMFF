/*
 * Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <gtest/gtest.h>

#include <ISOBMFF.hpp>
#include <utility>

namespace ISOBMFF {
namespace {
Error ReadConfig(DEC3& box, const std::vector<uint8_t>& data) {
  BinaryDataStream stream(data);
  Parser parser;
  return box.ReadData(parser, stream);
}
// A syntax generator, independent of the byte-oriented parser under test.
std::vector<uint8_t> Config(
    const std::vector<DEC3::IndependentSubstream>& subs) {
  std::vector<uint8_t> data;
  unsigned int used = 0;
  auto bits = [&](unsigned int value, unsigned int width) {
    for (unsigned int i = width; i > 0; --i) {
      if (used % 8 == 0) data.push_back(0);
      data.back() |=
          static_cast<uint8_t>(((value >> (i - 1)) & 1u) << (7 - used % 8));
      ++used;
    }
  };
  bits(768, 13);
  bits(static_cast<unsigned int>(subs.size() - 1), 3);
  for (const auto& s : subs) {
    bits(s.fscod, 2);
    bits(s.bsid, 5);
    bits(0, 1);
    bits(s.asvc ? 1 : 0, 1);
    bits(s.bsmod, 3);
    bits(s.acmod, 3);
    bits(s.lfeon ? 1 : 0, 1);
    bits(0, 3);
    bits(s.num_dep_sub, 4);
    if (s.num_dep_sub)
      bits(s.chan_loc, 9);
    else
      bits(0, 1);
  }
  return data;
}
}  // namespace

TEST(ISOBMFFDEC3Test, AndroidAtmosConfiguration) {
  // Exact dec3 payload from both Android Atmos WISP outputs (not box header).
  DEC3 box;
  ASSERT_FALSE(ReadConfig(box, {0x18, 0x00, 0x20, 0x0f, 0x00, 0x01, 0x0c}));
  EXPECT_EQ(768, box.GetDataRate());
  auto subs = box.GetIndependentSubstreams();
  ASSERT_EQ(1u, subs.size());
  EXPECT_EQ(0, subs[0].fscod);
  EXPECT_EQ(16, subs[0].bsid);
  EXPECT_FALSE(subs[0].asvc);
  EXPECT_EQ(0, subs[0].bsmod);
  EXPECT_EQ(7, subs[0].acmod);
  EXPECT_TRUE(subs[0].lfeon);
  EXPECT_EQ(0, subs[0].num_dep_sub);
  EXPECT_EQ(0, subs[0].chan_loc);
  EXPECT_EQ(6, subs[0].GetChannelCount());
  EXPECT_EQ(48000u, subs[0].GetSampleRate());
  EXPECT_TRUE(box.HasExtension());
  EXPECT_TRUE(box.GetFlagEC3ExtensionTypeA());
  EXPECT_EQ(12, box.GetComplexityIndexTypeA());
}

TEST(ISOBMFFDEC3Test, IndependentServicesAndDependentLocations) {
  DEC3::IndependentSubstream main;
  main.bsid = 16;
  main.acmod = 7;
  main.lfeon = true;
  main.num_dep_sub = 1;
  main.chan_loc = 2;  // Lrs/Rrs: two additional channels, not one.
  auto associated = main;
  associated.fscod = 1;
  associated.asvc = true;
  associated.bsmod = 3;
  associated.acmod = 1;
  associated.lfeon = false;
  associated.num_dep_sub = 0;
  DEC3 box;
  ASSERT_FALSE(ReadConfig(box, Config({main, associated})));
  auto subs = box.GetIndependentSubstreams();
  ASSERT_EQ(2u, subs.size());
  EXPECT_EQ(8, subs[0].GetChannelCount());
  EXPECT_EQ(1, subs[1].GetChannelCount());
  EXPECT_TRUE(subs[1].asvc);
  EXPECT_EQ(3, subs[1].bsmod);
  EXPECT_EQ(44100u, subs[1].GetSampleRate());
  EXPECT_FALSE(box.HasExtension());
  EXPECT_FALSE(box.GetFlagEC3ExtensionTypeA());
}

TEST(ISOBMFFDEC3Test, AllChannelLocationsAndModes) {
  const uint16_t counts[] = {2, 1, 2, 3, 3, 4, 4, 5};
  const uint16_t extra[] = {2, 2, 1, 1, 2, 2, 2, 1, 1};
  for (uint8_t acmod = 0; acmod < 8; ++acmod) {
    for (bool lfe : {false, true}) {
      for (unsigned int bit = 0; bit < 9; ++bit) {
        DEC3::IndependentSubstream sub;
        sub.acmod = acmod;
        sub.lfeon = lfe;
        sub.bsid = 16;
        sub.num_dep_sub = 15;
        sub.chan_loc = static_cast<uint16_t>(1u << bit);
        DEC3 box;
        ASSERT_FALSE(ReadConfig(box, Config({sub})));
        auto parsed = box.GetIndependentSubstreams()[0];
        EXPECT_EQ(sub.chan_loc, parsed.chan_loc);
        EXPECT_EQ(counts[acmod] + (lfe ? 1 : 0) + extra[bit],
                  parsed.GetChannelCount());
      }
    }
  }
}

TEST(ISOBMFFDEC3Test, EightServicesAndAllSampleRateCodes) {
  std::vector<DEC3::IndependentSubstream> subs(8);
  const uint32_t rates[] = {48000, 44100, 32000, 0};
  for (size_t i = 0; i < subs.size(); ++i) {
    subs[i].fscod = static_cast<uint8_t>(i % 4);
    subs[i].bsid = 16;
    subs[i].acmod = static_cast<uint8_t>(i);
  }
  DEC3 box;
  ASSERT_FALSE(ReadConfig(box, Config(subs)));
  auto parsed = box.GetIndependentSubstreams();
  ASSERT_EQ(8u, parsed.size());
  for (size_t i = 0; i < parsed.size(); ++i) {
    EXPECT_EQ(rates[i % 4], parsed[i].GetSampleRate());
    EXPECT_EQ(i, parsed[i].acmod);
  }
}

TEST(ISOBMFFDEC3Test, TruncatedMandatoryFieldsAndJocComplexity) {
  const std::vector<uint8_t> joc = {0x18, 0, 0x20, 0x0f, 0, 1, 12};
  for (size_t size = 0; size < joc.size(); ++size) {
    SCOPED_TRACE(size);
    DEC3 box;
    auto prefix = joc;
    prefix.resize(size);
    auto err = ReadConfig(box, prefix);
    // Five bytes is a complete base dec3; six declares JOC without complexity.
    EXPECT_EQ(size != 5, static_cast<bool>(err));
  }
  DEC3::IndependentSubstream sub;
  sub.num_dep_sub = 1;
  sub.chan_loc = 0x100;
  auto dependent = Config({sub});
  dependent.pop_back();
  DEC3 box;
  EXPECT_TRUE(ReadConfig(box, dependent));
  auto many = Config(std::vector<DEC3::IndependentSubstream>(8));
  many.pop_back();
  EXPECT_TRUE(ReadConfig(box, many));
}

TEST(ISOBMFFDEC3Test, ReservedExtensionBitsAndAdditionalData) {
  DEC3 box;
  ASSERT_FALSE(ReadConfig(box, {0x18, 0, 0x20, 0x0f, 0, 0x81, 16, 0xaa}));
  EXPECT_TRUE(box.GetFlagEC3ExtensionTypeA());
  EXPECT_EQ(16, box.GetComplexityIndexTypeA());
  EXPECT_EQ(std::vector<uint8_t>({0xaa}), box.GetAdditionalData());
  // Explicit flag zero has no complexity byte: following bytes stay opaque.
  ASSERT_FALSE(ReadConfig(box, {0x18, 0, 0x20, 0x0f, 0, 0x80, 0xaa}));
  EXPECT_TRUE(box.HasExtension());
  EXPECT_FALSE(box.GetFlagEC3ExtensionTypeA());
  EXPECT_EQ(0, box.GetComplexityIndexTypeA());
  EXPECT_EQ(std::vector<uint8_t>({0xaa}), box.GetAdditionalData());
}

TEST(ISOBMFFDEC3Test, CopyMoveAndRepeatedRead) {
  DEC3 box;
  ASSERT_FALSE(ReadConfig(box, {0x18, 0, 0x20, 0x0f, 0, 1, 12}));
  DEC3 copied(box);
  DEC3 moved(std::move(copied));
  DEC3 assigned;
  assigned = moved;
  EXPECT_EQ(12, assigned.GetComplexityIndexTypeA());
  ASSERT_EQ(1u, assigned.GetIndependentSubstreams().size());
  ASSERT_FALSE(ReadConfig(box, {0x18, 0, 0x20, 0x04, 0}));
  EXPECT_FALSE(box.HasExtension());
  EXPECT_FALSE(box.GetFlagEC3ExtensionTypeA());
  EXPECT_EQ(0, box.GetComplexityIndexTypeA());
  EXPECT_EQ(2, box.GetIndependentSubstreams()[0].GetChannelCount());
  EXPECT_EQ(12, assigned.GetComplexityIndexTypeA());
  EXPECT_FALSE(assigned.GetDisplayableProperties().empty());
}
}  // namespace ISOBMFF

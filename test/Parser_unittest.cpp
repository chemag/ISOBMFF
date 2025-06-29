/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */
#include <ISOBMFF.hpp>        // for various
#include <Parser.hpp> // for Parser

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace ISOBMFF {

class ISOBMFFParserTest : public ::testing::Test {
public:
  ISOBMFFParserTest() {}
  ~ISOBMFFParserTest() override {}
};

TEST_F(ISOBMFFParserTest, TestSampleVideo) {
  // fuzzer::conv: data
  const std::vector<uint8_t> &buffer = {
      // following example input is from a sample .mp4 video file
      // ftyp size: 24 bytes
      0x00, 0x00, 0x00, 0x18,
      // ftyp
      0x66, 0x74, 0x79, 0x70,
      // ftyp content
      0x6d, 0x70, 0x34, 0x32, 0x00, 0x00, 0x00, 0x00, 0x69, 0x73, 0x6f, 0x6d,
      0x6d, 0x70, 0x34, 0x32,
      // moov size: 2274 bytes
      0x00, 0x00, 0x08, 0xe2,
      // moov
      0x6d, 0x6f, 0x6f, 0x76,
      // mvhd size: 108 bytes
      0x00, 0x00, 0x00, 0x6c,
      // mvhd
      0x6d, 0x76, 0x68, 0x64,
      // mvhd content
      0x00, 0x00, 0x00, 0x00, 0xe4, 0x00, 0xb3, 0x34, 0xe4, 0x00, 0xb3, 0x34,
      0x00, 0x00, 0x27, 0x10, 0x00, 0x00, 0x37, 0x02, 0x00, 0x01, 0x00, 0x00,
      0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x03,
      // meta size: 118 bytes
      0x00, 0x00, 0x00, 0x76,
      // meta
      0x6d, 0x65, 0x74, 0x61,
      // meta content
      0x00, 0x00, 0x00, 0x21, 0x68, 0x64, 0x6c, 0x72, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x6d, 0x64, 0x74, 0x61, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x2b, 0x6b, 0x65, 0x79, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x01, 0x00, 0x00, 0x00, 0x1b, 0x6d, 0x64, 0x74, 0x61, 0x63, 0x6f, 0x6d,
      0x2e, 0x61, 0x6e, 0x64, 0x72, 0x6f, 0x69, 0x64, 0x2e, 0x76, 0x65, 0x72,
      0x73, 0x69, 0x6f, 0x6e, 0x00, 0x00, 0x00, 0x22, 0x69, 0x6c, 0x73, 0x74,
      0x00, 0x00, 0x00, 0x1a, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x12,
      0x64, 0x61, 0x74, 0x61, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
      0x31, 0x32,
      // trak size: 865 bytes
      0x00, 0x00, 0x03, 0x61,
      // trak
      0x74, 0x72, 0x61, 0x6b,
      // trak content
      0x00, 0x00, 0x00, 0x5c, 0x74, 0x6b, 0x68, 0x64, 0x00, 0x00, 0x00, 0x07,
      0xe4, 0x00, 0xb3, 0x34, 0xe4, 0x00, 0xb3, 0x34, 0x00, 0x00, 0x00, 0x01,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x37, 0x02, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
      0x06, 0xc0, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xfd,
      0x6d, 0x64, 0x69, 0x61, 0x00, 0x00, 0x00, 0x20, 0x6d, 0x64, 0x68, 0x64,
      0x00, 0x00, 0x00, 0x00, 0xe4, 0x00, 0xb3, 0x34, 0xe4, 0x00, 0xb3, 0x34,
      0x00, 0x01, 0x5f, 0x90, 0x00, 0x01, 0xef, 0x13, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x2c, 0x68, 0x64, 0x6c, 0x72, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x76, 0x69, 0x64, 0x65, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x56, 0x69, 0x64, 0x65,
      0x6f, 0x48, 0x61, 0x6e, 0x64, 0x6c, 0x65, 0x00, 0x00, 0x00, 0x02, 0xa9,
      0x6d, 0x69, 0x6e, 0x66, 0x00, 0x00, 0x00, 0x14, 0x76, 0x6d, 0x68, 0x64,
      0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x24, 0x64, 0x69, 0x6e, 0x66, 0x00, 0x00, 0x00, 0x1c,
      0x64, 0x72, 0x65, 0x66, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
      0x00, 0x00, 0x00, 0x0c, 0x75, 0x72, 0x6c, 0x20, 0x00, 0x00, 0x00, 0x01,
      0x00, 0x00, 0x02, 0x69, 0x73, 0x74, 0x62, 0x6c, 0x00, 0x00, 0x00, 0xed,
      0x73, 0x74, 0x73, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
      0x00, 0x00, 0x00, 0xdd, 0x68, 0x76, 0x63, 0x31, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0xc0, 0x09, 0x00,
      0x00, 0x48, 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x18,
      0xff, 0xff, 0x00, 0x00, 0x00, 0x87, 0x68, 0x76, 0x63, 0x43, 0x01, 0x01,
      0x60, 0x00, 0x00, 0x00, 0xb0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0xf0,
      0x00, 0xfc, 0xfd, 0xf8, 0xf8, 0x00, 0x00, 0x03, 0x03, 0xa0, 0x00, 0x01,
      0x00, 0x20, 0x40, 0x01, 0x0c, 0x01, 0xff, 0xff, 0x01, 0x60, 0x00, 0x00,
      0x03, 0x00, 0xb0, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x99, 0xac,
      0x0c, 0x07, 0xf2, 0x81, 0x54, 0xee, 0x6b, 0x28, 0x01, 0x40, 0xa1, 0x00,
      0x01, 0x00, 0x2e, 0x42, 0x01, 0x01, 0x01, 0x60, 0x00, 0x00, 0x03, 0x00,
      0xb0, 0x00, 0x00, 0x03, 0x00, 0x00, 0x03, 0x00, 0x99, 0xa0, 0x03, 0x60,
      0x80, 0x09, 0x01, 0x65, 0xae, 0xe4, 0xc9, 0x2e, 0xa6, 0xa1, 0x22, 0x41,
      0x20, 0x80, 0xfe, 0x50, 0x2a, 0x9d, 0xcd, 0x65, 0x00, 0x17, 0x68, 0x50,
      0x94, 0xa2, 0x00, 0x01, 0x00, 0x0b, 0x44, 0x01, 0xc0, 0xe3, 0x0f, 0x09,
      0xc1, 0x50, 0xaf, 0xb0, 0x84, 0x00, 0x00, 0x00, 0x58, 0x73, 0x74, 0x74,
      0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x00, 0x00, 0x00,
      0x08, 0x00, 0x00, 0x0b, 0xb8, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x0e,
      0xa2, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 0x0b, 0xb8, 0x00, 0x00, 0x00,
      0x02, 0x00, 0x00, 0x0b, 0xaf, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0b,
      0xc0, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0b, 0xb0, 0x00, 0x00, 0x00,
      0x02, 0x00, 0x00, 0x0b, 0xbf, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0b,
      0xb0, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x0b, 0xbf, 0x00, 0x00, 0x00,
      0x18, 0x73, 0x74, 0x73, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x02, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00,
      0xbc, 0x73, 0x74, 0x73, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x2a, 0x00, 0x01, 0xb3, 0x7d, 0x00, 0x00, 0x15,
      0x72, 0x00, 0x00, 0x1f, 0x5d, 0x00, 0x00, 0x21, 0xca, 0x00, 0x00, 0x32,
      0x5a, 0x00, 0x00, 0x9b, 0x8e, 0x00, 0x00, 0xb7, 0xbd, 0x00, 0x00, 0xbf,
      0x56, 0x00, 0x00, 0xf7, 0x26, 0x00, 0x00, 0xe7, 0x5e, 0x00, 0x00, 0xcd,
      0x9b, 0x00, 0x00, 0xc7, 0xce, 0x00, 0x00, 0xd3, 0x6c, 0x00, 0x00, 0xc9,
      0x47, 0x00, 0x00, 0xcb, 0x4f, 0x00, 0x00, 0xc5, 0x0a, 0x00, 0x00, 0xc2,
      0x67, 0x00, 0x00, 0xc2, 0xac, 0x00, 0x00, 0xcf, 0x86, 0x00, 0x00, 0xd6,
      0x43, 0x00, 0x01, 0x0f, 0xbb, 0x00, 0x00, 0xff, 0xed, 0x00, 0x00, 0xc0,
      0x1e, 0x00, 0x00, 0xfc, 0xbd, 0x00, 0x00, 0xed, 0x50, 0x00, 0x00, 0xe4,
      0xe8, 0x00, 0x00, 0xb0, 0xe5, 0x00, 0x00, 0xb3, 0x09, 0x00, 0x00, 0xf9,
      0xb9, 0x00, 0x00, 0xf4, 0xa9, 0x00, 0x02, 0x21, 0x19, 0x00, 0x00, 0xbb,
      0x5f, 0x00, 0x00, 0xd0, 0x3e, 0x00, 0x00, 0xfe, 0xb1, 0x00, 0x00, 0xd5,
      0x91, 0x00, 0x01, 0x17, 0xf5, 0x00, 0x01, 0x13, 0x1d, 0x00, 0x00, 0xd6,
      0x3d, 0x00, 0x00, 0xdf, 0x79, 0x00, 0x01, 0x01, 0xc0, 0x00, 0x00, 0xe3,
      0xf9, 0x00, 0x01, 0x0f, 0x5c, 0x00, 0x00, 0x00, 0x28, 0x73, 0x74, 0x73,
      0x63, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00,
      0x01, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
      0x02, 0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
      0x20, 0x63, 0x6f, 0x36, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0xa0, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x1a, 0x36, 0x48,
      // trak size: 1175 bytes
      0x00, 0x00, 0x04, 0x97,
      // trak
      0x74, 0x72, 0x61, 0x6b,
      // trak content
      0x00, 0x00, 0x00, 0x5c, 0x74, 0x6b, 0x68, 0x64, 0x00, 0x00, 0x00, 0x07,
      0xe4, 0x00, 0xb3, 0x34, 0xe4, 0x00, 0xb3, 0x34, 0x00, 0x00, 0x00, 0x02,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x35, 0x9b, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00,
      0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30,
      0x65, 0x64, 0x74, 0x73, 0x00, 0x00, 0x00, 0x28, 0x65, 0x6c, 0x73, 0x74,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x05, 0x60,
      0xff, 0xff, 0xff, 0xff, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x30, 0x3b,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03,
      0x6d, 0x64, 0x69, 0x61, 0x00, 0x00, 0x00, 0x20, 0x6d, 0x64, 0x68, 0x64,
      0x00, 0x00, 0x00, 0x00, 0xe4, 0x00, 0xb3, 0x34, 0xe4, 0x00, 0xb3, 0x34,
      0x00, 0x00, 0xbb, 0x80, 0x00, 0x01, 0x01, 0x4e, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x2c, 0x68, 0x64, 0x6c, 0x72, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x73, 0x6f, 0x75, 0x6e, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x53, 0x6f, 0x75, 0x6e,
      0x64, 0x48, 0x61, 0x6e, 0x64, 0x6c, 0x65, 0x00, 0x00, 0x00, 0x03, 0xaf,
      0x6d, 0x69, 0x6e, 0x66, 0x00, 0x00, 0x00, 0x10, 0x73, 0x6d, 0x68, 0x64,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24,
      0x64, 0x69, 0x6e, 0x66, 0x00, 0x00, 0x00, 0x1c, 0x64, 0x72, 0x65, 0x66,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0c,
      0x75, 0x72, 0x6c, 0x20, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x73,
      0x73, 0x74, 0x62, 0x6c, 0x00, 0x00, 0x00, 0x5b, 0x73, 0x74, 0x73, 0x64,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x4b,
      0x6d, 0x70, 0x34, 0x61, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x10,
      0x00, 0x00, 0x00, 0x00, 0xbb, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x27,
      0x65, 0x73, 0x64, 0x73, 0x00, 0x00, 0x00, 0x00, 0x03, 0x19, 0x00, 0x00,
      0x00, 0x04, 0x11, 0x40, 0x15, 0x00, 0x03, 0x00, 0x00, 0x01, 0xf4, 0x00,
      0x00, 0x01, 0xf4, 0x00, 0x05, 0x02, 0x11, 0x90, 0x06, 0x01, 0x02, 0x00,
      0x00, 0x01, 0xd0, 0x73, 0x74, 0x74, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x38, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x4d, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x8c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x0c, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x4d, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x05, 0x0d, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x8d, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x11, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x4f, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x05, 0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x95, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x50, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x05, 0x0f, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x8d, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x0e, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0xcf, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x05, 0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x0a, 0x91, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x13, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x02, 0xcf, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0xcc, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x0f, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x4e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x05, 0x0b, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x8e, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x0e, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x04, 0x4e, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x80, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x01, 0x00, 0x00, 0x05, 0x0e, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x04, 0x94, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x03, 0x00, 0x00,
      0x00, 0x00, 0x02, 0x00, 0x00, 0x04, 0x0e, 0x00, 0x00, 0x00, 0xf8, 0x73,
      0x74, 0x73, 0x7a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x39, 0x00, 0x00, 0x01, 0x55, 0x00, 0x00, 0x01, 0xd7, 0x00,
      0x00, 0x01, 0xce, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x01, 0xa4, 0x00,
      0x00, 0x01, 0xaa, 0x00, 0x00, 0x01, 0x87, 0x00, 0x00, 0x01, 0x81, 0x00,
      0x00, 0x01, 0x72, 0x00, 0x00, 0x01, 0x6c, 0x00, 0x00, 0x01, 0x71, 0x00,
      0x00, 0x01, 0x6d, 0x00, 0x00, 0x01, 0x63, 0x00, 0x00, 0x01, 0x5d, 0x00,
      0x00, 0x01, 0x53, 0x00, 0x00, 0x01, 0x44, 0x00, 0x00, 0x01, 0x45, 0x00,
      0x00, 0x01, 0x38, 0x00, 0x00, 0x01, 0x38, 0x00, 0x00, 0x01, 0x3f, 0x00,
      0x00, 0x01, 0x3f, 0x00, 0x00, 0x01, 0x3f, 0x00, 0x00, 0x01, 0x38, 0x00,
      0x00, 0x01, 0x32, 0x00, 0x00, 0x01, 0x3c, 0x00, 0x00, 0x01, 0x47, 0x00,
      0x00, 0x01, 0x3b, 0x00, 0x00, 0x01, 0x51, 0x00, 0x00, 0x01, 0x48, 0x00,
      0x00, 0x01, 0x40, 0x00, 0x00, 0x01, 0x4a, 0x00, 0x00, 0x01, 0x45, 0x00,
      0x00, 0x01, 0x59, 0x00, 0x00, 0x01, 0x5c, 0x00, 0x00, 0x01, 0x56, 0x00,
      0x00, 0x01, 0x49, 0x00, 0x00, 0x01, 0x5d, 0x00, 0x00, 0x01, 0x45, 0x00,
      0x00, 0x01, 0x4d, 0x00, 0x00, 0x01, 0x44, 0x00, 0x00, 0x01, 0x51, 0x00,
      0x00, 0x01, 0x52, 0x00, 0x00, 0x01, 0x50, 0x00, 0x00, 0x01, 0x4d, 0x00,
      0x00, 0x01, 0x52, 0x00, 0x00, 0x01, 0x62, 0x00, 0x00, 0x01, 0x4b, 0x00,
      0x00, 0x01, 0x54, 0x00, 0x00, 0x01, 0x4b, 0x00, 0x00, 0x01, 0x4b, 0x00,
      0x00, 0x01, 0x5c, 0x00, 0x00, 0x01, 0x61, 0x00, 0x00, 0x01, 0x4e, 0x00,
      0x00, 0x01, 0x66, 0x00, 0x00, 0x01, 0x57, 0x00, 0x00, 0x01, 0x48, 0x00,
      0x00, 0x01, 0x54, 0x00, 0x00, 0x00, 0x28, 0x73, 0x74, 0x73, 0x63, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x01, 0x00,
      0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00,
      0x00, 0x00, 0x09, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x20, 0x63,
      0x6f, 0x36, 0x34, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x19, 0xf5, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x23, 0xb0, 0xa5,
      // free size: 918 bytes
      0x00, 0x00, 0x03, 0x96,
      // free
      0x66, 0x72, 0x65, 0x65,
      // free content
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
      // mdat size: 1 byte
      // 0x00, 0x00, 0x00, 0x01,
      // mdat
      // 0x6d, 0x64, 0x61, 0x74
  };
  // fuzzer::conv: begin
  ISOBMFF::Parser parser;
  try {
    parser.Parse(buffer);
  } catch (std::exception &e) {
    fprintf(stderr, "Caught exception: %s\n", e.what());
  }
  // fuzzer::conv: end

  std::shared_ptr<ISOBMFF::File> file = parser.GetFile();
  EXPECT_TRUE(file != nullptr);

  // validate moov box
  std::shared_ptr<ISOBMFF::ContainerBox> moov =
      file->GetTypedBox<ISOBMFF::ContainerBox>("moov");
  EXPECT_TRUE(moov != nullptr);

  // loop for trak
  for (auto &box : moov->GetBoxes()) {
    std::string name = box->GetName();
    if (name.compare("trak") != 0) {
      continue;
    }

    // validate mdia box
    auto trak = std::dynamic_pointer_cast<ISOBMFF::ContainerBox>(box);
    std::shared_ptr<ISOBMFF::ContainerBox> mdia =
        trak->GetTypedBox<ISOBMFF::ContainerBox>("mdia");
    EXPECT_TRUE(mdia != nullptr);

    // validate hdlr box
    std::shared_ptr<ISOBMFF::HDLR> hdlr =
        mdia->GetTypedBox<ISOBMFF::HDLR>("hdlr");
    EXPECT_TRUE(hdlr != nullptr);

    // validate mdhd box
    std::shared_ptr<ISOBMFF::MDHD> mdhd =
        mdia->GetTypedBox<ISOBMFF::MDHD>("mdhd");
    EXPECT_TRUE(mdhd != nullptr);

    // validate minf box
    std::shared_ptr<ISOBMFF::ContainerBox> minf =
        mdia->GetTypedBox<ISOBMFF::ContainerBox>("minf");
    EXPECT_TRUE(minf != nullptr);

    // validate stbl box
    std::shared_ptr<ISOBMFF::ContainerBox> stbl =
        minf->GetTypedBox<ISOBMFF::ContainerBox>("stbl");
    EXPECT_TRUE(stbl != nullptr);

    // validate tkhd box
    std::shared_ptr<ISOBMFF::TKHD> tkhd =
        trak->GetTypedBox<ISOBMFF::TKHD>("tkhd");
    EXPECT_TRUE(tkhd != nullptr);
  }
}

} // namespace ISOBMFF

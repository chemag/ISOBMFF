/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @header      AV2CInBand.hpp
 */

#ifndef ISOBMFF_AV2CINBAND_HPP
#define ISOBMFF_AV2CINBAND_HPP

#include <Box.hpp>
#include <Macros.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ISOBMFF {
// InBandAV2CodecConfigurationBox ("av2c"), as defined in "AV2 Codec ISO Media
// File Format Binding" (working draft), section 3.2.2. Note that this is a
// Box, not a FullBox. Its 16 bits are reserved1 (3), max_num_streams_minus1
// (2), max_profile_idc (5), max_level_idx (5), and max_tier (1).
class ISOBMFF_EXPORT AV2CInBand : public Box {
 public:
  AV2CInBand();
  AV2CInBand(const AV2CInBand& o);
  AV2CInBand(AV2CInBand&& o) noexcept;
  virtual ~AV2CInBand() override;

  AV2CInBand& operator=(AV2CInBand o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;

  uint8_t GetReserved1() const;
  uint8_t GetMaxNumStreamsMinus1() const;
  uint8_t GetMaxProfileIdc() const;
  uint8_t GetMaxLevelIdx() const;
  uint8_t GetMaxTier() const;

  void SetReserved1(uint8_t value);
  void SetMaxNumStreamsMinus1(uint8_t value);
  void SetMaxProfileIdc(uint8_t value);
  void SetMaxLevelIdx(uint8_t value);
  void SetMaxTier(uint8_t value);

  ISOBMFF_EXPORT friend void swap(AV2CInBand& o1, AV2CInBand& o2);

 private:
  class IMPL;

  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_AV2CINBAND_HPP */

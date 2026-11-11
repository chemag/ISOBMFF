/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @header      AV2COutOfBand.hpp
 */

#ifndef ISOBMFF_AV2COUTOFBAND_HPP
#define ISOBMFF_AV2COUTOFBAND_HPP

#include <Box.hpp>
#include <Macros.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ISOBMFF {
// OutOfBandAV2CodecConfigurationBox ("av2C"), as defined in "AV2 Codec ISO
// Media File Format Binding" (working draft), section 3.2.2. Note that this is
// a Box, not a FullBox. It carries config_obus_count_minus1 + 1 OBUs, each one
// preceded by its leb128() length (Annex B, Length delimited bitstream format,
// of the AV2 specification). The OBUs are stored without that length.
class ISOBMFF_EXPORT AV2COutOfBand : public Box {
 public:
  AV2COutOfBand();
  AV2COutOfBand(const AV2COutOfBand& o);
  AV2COutOfBand(AV2COutOfBand&& o) noexcept;
  virtual ~AV2COutOfBand() override;

  AV2COutOfBand& operator=(AV2COutOfBand o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;

  uint8_t GetReserved1() const;
  uint8_t GetConfigOBUsCountMinus1() const;
  std::vector<std::vector<uint8_t> > GetConfigOBUs() const;

  void SetReserved1(uint8_t value);
  void SetConfigOBUsCountMinus1(uint8_t value);
  void SetConfigOBUs(const std::vector<std::vector<uint8_t> >& value);

  ISOBMFF_EXPORT friend void swap(AV2COutOfBand& o1, AV2COutOfBand& o2);

 private:
  class IMPL;

  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_AV2COUTOFBAND_HPP */

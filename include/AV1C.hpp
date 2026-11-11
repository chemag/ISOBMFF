/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @header      AV1C.hpp
 */

#ifndef ISOBMFF_AV1C_HPP
#define ISOBMFF_AV1C_HPP

#include <Box.hpp>
#include <Macros.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ISOBMFF {
// AV1CodecConfigurationBox ("av1C"), as defined in "AV1 Codec ISO Media File
// Format Binding", section 2.3. Note that this is a Box, not a FullBox. Its
// payload is an AV1CodecConfigurationRecord: 4 bytes of fields, followed by
// configOBUs, which run to the end of the box.
class ISOBMFF_EXPORT AV1C : public Box {
 public:
  AV1C();
  AV1C(const AV1C& o);
  AV1C(AV1C&& o) noexcept;
  virtual ~AV1C() override;

  AV1C& operator=(AV1C o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;

  uint8_t GetMarker() const;
  uint8_t GetVersion() const;
  uint8_t GetSeqProfile() const;
  uint8_t GetSeqLevelIdx0() const;
  uint8_t GetSeqTier0() const;
  uint8_t GetHighBitdepth() const;
  uint8_t GetTwelveBit() const;
  uint8_t GetMonochrome() const;
  uint8_t GetChromaSubsamplingX() const;
  uint8_t GetChromaSubsamplingY() const;
  uint8_t GetChromaSamplePosition() const;
  uint8_t GetInitialPresentationDelayPresent() const;
  uint8_t GetInitialPresentationDelayMinusOne() const;
  std::vector<uint8_t> GetConfigOBUs() const;

  void SetMarker(uint8_t value);
  void SetVersion(uint8_t value);
  void SetSeqProfile(uint8_t value);
  void SetSeqLevelIdx0(uint8_t value);
  void SetSeqTier0(uint8_t value);
  void SetHighBitdepth(uint8_t value);
  void SetTwelveBit(uint8_t value);
  void SetMonochrome(uint8_t value);
  void SetChromaSubsamplingX(uint8_t value);
  void SetChromaSubsamplingY(uint8_t value);
  void SetChromaSamplePosition(uint8_t value);
  void SetInitialPresentationDelayPresent(uint8_t value);
  void SetInitialPresentationDelayMinusOne(uint8_t value);
  void SetConfigOBUs(const std::vector<uint8_t>& value);

  ISOBMFF_EXPORT friend void swap(AV1C& o1, AV1C& o2);

 private:
  class IMPL;

  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_AV1C_HPP */

/*
 * Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#ifndef ISOBMFF_DEC3_HPP
#define ISOBMFF_DEC3_HPP

#include <Box.hpp>
#include <cstdint>
#include <memory>
#include <vector>

namespace ISOBMFF {
// EC3SpecificBox: ETSI TS 102 366 V1.4.1, Annex F.6, with the
// backward-compatible Dolby Digital Plus Online Delivery Kit Atmos extension.
// This is container signalling, not a validation of the elementary stream.
class ISOBMFF_EXPORT DEC3 : public Box {
 public:
  struct IndependentSubstream {
    uint8_t fscod = 0;
    uint8_t bsid = 0;
    bool asvc = false;
    uint8_t bsmod = 0;
    uint8_t acmod = 0;
    bool lfeon = false;
    uint8_t num_dep_sub = 0;
    uint16_t chan_loc = 0;

    // fscod == 3 requires fscod2 from the bitstream: dec3 cannot resolve it.
    uint32_t GetSampleRate() const;
    // Channels in this audio service, including unique dependent locations.
    // Independent services are not summed together.
    uint16_t GetChannelCount() const;
  };

  DEC3();
  DEC3(const DEC3& o);
  DEC3(DEC3&& o) noexcept;
  ~DEC3() override;
  DEC3& operator=(DEC3 o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;

  uint16_t GetDataRate() const;  // kbit/s; maximum for variable-rate streams
  std::vector<IndependentSubstream> GetIndependentSubstreams() const;
  bool HasExtension() const;
  bool GetFlagEC3ExtensionTypeA() const;
  uint8_t GetComplexityIndexTypeA() const;  // only meaningful when flag is set
  std::vector<uint8_t> GetAdditionalData() const;

  ISOBMFF_EXPORT friend void swap(DEC3& o1, DEC3& o2);

 private:
  class IMPL;
  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_DEC3_HPP */

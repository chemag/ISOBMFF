/*
 * Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#ifndef ISOBMFF_EC3_HPP
#define ISOBMFF_EC3_HPP

#include <ContainerBox.hpp>
#include <cstdint>
#include <memory>

namespace ISOBMFF {
// EC3SampleEntry (ETSI TS 102 366 Annex F.5). This is not a FullBox.
// The raw ChannelCount and SampleSize fields are placeholders, not the
// decoded channel count or PCM precision; applications must ignore them.
class ISOBMFF_EXPORT EC3 : public ContainerBox {
 public:
  EC3();
  EC3(const EC3& o);
  EC3(EC3&& o) noexcept;
  ~EC3() override;
  EC3& operator=(EC3 o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;
  uint16_t GetDataReferenceIndex() const;
  uint16_t GetChannelCount() const;  // raw sample-entry placeholder
  uint16_t GetSampleSize() const;    // raw sample-entry placeholder
  uint32_t GetSampleRateRaw() const;
  ISOBMFF_EXPORT friend void swap(EC3& o1, EC3& o2);

 private:
  class IMPL;
  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_EC3_HPP */

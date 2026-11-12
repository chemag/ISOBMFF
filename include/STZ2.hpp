/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @header      STZ2.hpp
 */

#ifndef ISOBMFF_STZ2_HPP
#define ISOBMFF_STZ2_HPP

#include <FullBox.hpp>
#include <Macros.hpp>
#include <algorithm>
#include <memory>
#include <string>

namespace ISOBMFF {
// CompactSampleSizeBox ("stz2"), ISO/IEC 14496-12:2026, section 8.7.3.3.
// The compact variant of the SampleSizeBox ("stsz"): it gives the size in bytes
// of every sample in the track, and the number of samples in the track, with
// one entry_size per sample stored in 4, 8, or 16 bits (field_size). There is
// no constant-size form.
class ISOBMFF_EXPORT STZ2 : public FullBox {
 public:
  STZ2();
  STZ2(const STZ2& o);
  STZ2(STZ2&& o) noexcept;
  virtual ~STZ2() override;

  STZ2& operator=(STZ2 o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;

  // the size in bits of each entry_size: 4, 8, or 16
  uint8_t GetFieldSize() const;
  // the number of samples in the track
  uint32_t GetSampleCount() const;
  // the number of entry_size values read (sample_count)
  size_t GetEntryCount() const;
  uint16_t GetEntrySize(size_t index) const;

  ISOBMFF_EXPORT friend void swap(STZ2& o1, STZ2& o2);

 private:
  class IMPL;

  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_STZ2_HPP */

/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @header      STSZ.hpp
 */

#ifndef ISOBMFF_STSZ_HPP
#define ISOBMFF_STSZ_HPP

#include <FullBox.hpp>
#include <Macros.hpp>
#include <algorithm>
#include <memory>
#include <string>

namespace ISOBMFF {
// SampleSizeBox ("stsz"), ISO/IEC 14496-12:2026, section 8.7.3.2. Gives the
// size in bytes of every sample in the track, and the number of samples in
// the track. Either all samples have the same size (sample_size != 0, and
// there is no per-sample table), or there is one entry_size per sample.
class ISOBMFF_EXPORT STSZ : public FullBox {
 public:
  STSZ();
  STSZ(const STSZ& o);
  STSZ(STSZ&& o) noexcept;
  virtual ~STSZ() override;

  STSZ& operator=(STSZ o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;

  // the size shared by all samples, or 0 when the sizes are in the table
  uint32_t GetSampleSize() const;
  // the number of samples in the track
  uint32_t GetSampleCount() const;
  // the number of entry_size values read: sample_count when sample_size is
  // 0, and 0 otherwise
  size_t GetEntryCount() const;
  uint32_t GetEntrySize(size_t index) const;

  ISOBMFF_EXPORT friend void swap(STSZ& o1, STSZ& o2);

 private:
  class IMPL;

  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_STSZ_HPP */

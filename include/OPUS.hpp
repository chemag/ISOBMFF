/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @header      OPUS.hpp
 */

#ifndef ISOBMFF_OPUS_HPP
#define ISOBMFF_OPUS_HPP

#include <Container.hpp>
#include <FullBox.hpp>
#include <Macros.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>

namespace ISOBMFF {
// Opus sample entry ("Opus"), as defined in "Encapsulation of Opus in ISO
// Base Media File Format" (an AudioSampleEntry containing a "dOps" box).
class ISOBMFF_EXPORT OPUS : public FullBox, public Container {
 public:
  OPUS();
  OPUS(const OPUS& o);
  OPUS(OPUS&& o) noexcept;
  virtual ~OPUS() override;

  OPUS& operator=(OPUS o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  void WriteDescription(std::ostream& os,
                        std::size_t indentLevel) const override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;

  uint16_t GetDataReferenceIndex() const;
  uint16_t GetChannelCount() const;
  uint16_t GetSampleSize() const;
  uint32_t GetSampleRateRaw() const;
  float GetSampleRate() const;

  void SetDataReferenceIndex(uint16_t value);
  void SetChannelCount(uint16_t value);
  void SetSampleSize(uint16_t value);
  void SetSampleRateRaw(uint32_t value);

  void AddBox(std::shared_ptr<Box> box) override;
  std::vector<std::shared_ptr<Box> > GetBoxes() const override;
  ISOBMFF_EXPORT friend void swap(OPUS& o1, OPUS& o2);

 private:
  class IMPL;

  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_OPUS_HPP */

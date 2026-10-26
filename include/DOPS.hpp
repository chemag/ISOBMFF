/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*!
 * @header      DOPS.hpp
 */

#ifndef ISOBMFF_DOPS_HPP
#define ISOBMFF_DOPS_HPP

#include <Box.hpp>
#include <Macros.hpp>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace ISOBMFF {
// OpusSpecificBox ("dOps"), as defined in "Encapsulation of Opus in ISO Base
// Media File Format". Note that this is a Box, not a FullBox.
class ISOBMFF_EXPORT DOPS : public Box {
 public:
  DOPS();
  DOPS(const DOPS& o);
  DOPS(DOPS&& o) noexcept;
  virtual ~DOPS() override;

  DOPS& operator=(DOPS o);

  Error ReadData(Parser& parser, BinaryStream& stream) override;
  std::vector<std::pair<std::string, std::string> > GetDisplayableProperties()
      const override;

  uint8_t GetVersion() const;
  uint8_t GetOutputChannelCount() const;
  uint16_t GetPreSkip() const;
  uint32_t GetInputSampleRate() const;
  int16_t GetOutputGain() const;
  uint8_t GetChannelMappingFamily() const;
  uint8_t GetStreamCount() const;
  uint8_t GetCoupledCount() const;
  std::vector<uint8_t> GetChannelMapping() const;

  void SetVersion(uint8_t value);
  void SetOutputChannelCount(uint8_t value);
  void SetPreSkip(uint16_t value);
  void SetInputSampleRate(uint32_t value);
  void SetOutputGain(int16_t value);
  void SetChannelMappingFamily(uint8_t value);
  void SetStreamCount(uint8_t value);
  void SetCoupledCount(uint8_t value);
  void SetChannelMapping(const std::vector<uint8_t>& value);

  ISOBMFF_EXPORT friend void swap(DOPS& o1, DOPS& o2);

 private:
  class IMPL;

  std::unique_ptr<IMPL> impl;
};
}  // namespace ISOBMFF

#endif /* ISOBMFF_DOPS_HPP */

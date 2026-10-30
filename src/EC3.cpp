/*
 * Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <DEC3.hpp>
#include <EC3.hpp>
#include <utility>

namespace ISOBMFF {
class EC3::IMPL {
 public:
  uint16_t data_reference_index = 0;
  uint16_t channel_count = 0;
  uint16_t sample_size = 0;
  uint32_t sample_rate = 0;
};
EC3::EC3() : ContainerBox("ec-3"), impl(std::make_unique<IMPL>()) {}
EC3::EC3(const EC3& o)
    : ContainerBox(o), impl(std::make_unique<IMPL>(*o.impl)) {}
EC3::EC3(EC3&& o) noexcept
    : ContainerBox(std::move(o)), impl(std::move(o.impl)) {}
EC3::~EC3() = default;
EC3& EC3::operator=(EC3 o) {
  swap(*this, o);
  return *this;
}
void swap(EC3& o1, EC3& o2) {
  using std::swap;
  swap(static_cast<ContainerBox&>(o1), static_cast<ContainerBox&>(o2));
  swap(o1.impl, o2.impl);
}
Error EC3::ReadData(Parser& parser, BinaryStream& stream) {
  IMPL state;
  uint8_t reserved8;
  uint16_t reserved16;
  uint32_t reserved32;
  Error err;
  for (unsigned int i = 0; i < 6; ++i) {
    err = stream.ReadUInt8(reserved8);
    if (err) return err;
  }
  err = stream.ReadBigEndianUInt16(state.data_reference_index);
  if (err) return err;
  for (unsigned int i = 0; i < 2; ++i) {
    err = stream.ReadBigEndianUInt32(reserved32);
    if (err) return err;
  }
  err = stream.ReadBigEndianUInt16(state.channel_count);
  if (err) return err;
  err = stream.ReadBigEndianUInt16(state.sample_size);
  if (err) return err;
  err = stream.ReadBigEndianUInt16(reserved16);
  if (err) return err;
  err = stream.ReadBigEndianUInt16(reserved16);
  if (err) return err;
  err = stream.ReadBigEndianUInt32(state.sample_rate);
  if (err) return err;
  err = ContainerBox::ReadData(parser, stream);
  if (err) return err;
  unsigned int configurations = 0;
  for (const auto& box : GetBoxes()) {
    if (box->GetName() == "dec3") {
      if (!std::dynamic_pointer_cast<DEC3>(box)) {
        return Error(ErrorCode::InvalidBoxData,
                     "ec-3 requires a typed dec3 box");
      }
      ++configurations;
    }
  }
  if (configurations != 1) {
    return Error(ErrorCode::InvalidBoxData,
                 "ec-3 requires exactly one dec3 box");
  }
  *impl = state;
  return Error();
}
uint16_t EC3::GetDataReferenceIndex() const {
  return impl->data_reference_index;
}
uint16_t EC3::GetChannelCount() const { return impl->channel_count; }
uint16_t EC3::GetSampleSize() const { return impl->sample_size; }
uint32_t EC3::GetSampleRateRaw() const { return impl->sample_rate; }
std::vector<std::pair<std::string, std::string> >
EC3::GetDisplayableProperties() const {
  auto props = ContainerBox::GetDisplayableProperties();
  props.push_back(
      {"Data Reference Index", std::to_string(GetDataReferenceIndex())});
  props.push_back(
      {"Channel Count (placeholder)", std::to_string(GetChannelCount())});
  props.push_back(
      {"Sample Size (placeholder)", std::to_string(GetSampleSize())});
  props.push_back({"Sample Rate (raw)", std::to_string(GetSampleRateRaw())});
  return props;
}
}  // namespace ISOBMFF

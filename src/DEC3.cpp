/*
 * Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

#include <DEC3.hpp>
#include <Parser.hpp>
#include <utility>

namespace ISOBMFF {
class DEC3::IMPL {
 public:
  uint16_t data_rate = 0;
  std::vector<IndependentSubstream> substreams;
  bool has_extension = false;
  bool extension_type_a = false;
  uint8_t complexity_index_type_a = 0;
  std::vector<uint8_t> additional_data;
};

DEC3::DEC3() : Box("dec3"), impl(std::make_unique<IMPL>()) {}
DEC3::DEC3(const DEC3& o) : Box(o), impl(std::make_unique<IMPL>(*o.impl)) {}
DEC3::DEC3(DEC3&& o) noexcept : Box(std::move(o)), impl(std::move(o.impl)) {}
DEC3::~DEC3() = default;
DEC3& DEC3::operator=(DEC3 o) {
  swap(*this, o);
  return *this;
}
void swap(DEC3& o1, DEC3& o2) {
  using std::swap;
  swap(static_cast<Box&>(o1), static_cast<Box&>(o2));
  swap(o1.impl, o2.impl);
}

Error DEC3::ReadData(Parser& parser, BinaryStream& stream) {
  (void)parser;
  // Parse into fresh state: repeated reads cannot retain earlier JOC or
  // services.
  IMPL state;
  uint16_t header;
  Error err = stream.ReadBigEndianUInt16(header);
  if (err) return err;
  state.data_rate = static_cast<uint16_t>(header >> 3);
  const unsigned int count = (header & 7u) + 1u;
  for (unsigned int i = 0; i < count; ++i) {
    uint8_t a, b, c;
    err = stream.ReadUInt8(a);
    if (err) return err;
    err = stream.ReadUInt8(b);
    if (err) return err;
    err = stream.ReadUInt8(c);
    if (err) return err;
    IndependentSubstream sub;
    sub.fscod = static_cast<uint8_t>(a >> 6);
    sub.bsid = static_cast<uint8_t>((a >> 1) & 31u);
    sub.asvc = (b & 0x80u) != 0;
    sub.bsmod = static_cast<uint8_t>((b >> 4) & 7u);
    sub.acmod = static_cast<uint8_t>((b >> 1) & 7u);
    sub.lfeon = (b & 1u) != 0;
    sub.num_dep_sub = static_cast<uint8_t>((c >> 1) & 15u);
    if (sub.num_dep_sub != 0) {
      uint8_t low;
      err = stream.ReadUInt8(low);
      if (err) return err;
      sub.chan_loc = static_cast<uint16_t>(((c & 1u) << 8) | low);
    }
    state.substreams.push_back(sub);
  }
  // The base Annex F syntax ends here. Older boxes need not contain the
  // extension byte. Reserved bits and future trailing data are ignored, not
  // interpreted as additional services or used to infer Atmos.
  if (stream.HasBytesAvailable()) {
    uint8_t flags;
    err = stream.ReadUInt8(flags);
    if (err) return err;
    state.has_extension = true;
    state.extension_type_a = (flags & 1u) != 0;
    if (state.extension_type_a) {
      err = stream.ReadUInt8(state.complexity_index_type_a);
      if (err) return err;
    }
    err = stream.ReadAllData(state.additional_data);
    if (err) return err;
  }
  *impl = std::move(state);
  return Error();
}

uint32_t DEC3::IndependentSubstream::GetSampleRate() const {
  const uint32_t rates[] = {48000, 44100, 32000, 0};
  return fscod < 4 ? rates[fscod] : 0;
}
uint16_t DEC3::IndependentSubstream::GetChannelCount() const {
  const uint16_t base[] = {2, 1, 2, 3, 3, 4, 4, 5};
  const uint16_t extra[] = {2, 2, 1, 1, 2, 2, 2, 1, 1};
  uint16_t count = acmod < 8 ? base[acmod] : 0;
  if (lfeon) ++count;
  if (num_dep_sub != 0) {
    for (unsigned int i = 0; i < 9; ++i) {
      if ((chan_loc & (1u << i)) != 0) count += extra[i];
    }
  }
  return count;
}

uint16_t DEC3::GetDataRate() const { return impl->data_rate; }
std::vector<DEC3::IndependentSubstream> DEC3::GetIndependentSubstreams() const {
  return impl->substreams;
}
bool DEC3::HasExtension() const { return impl->has_extension; }
bool DEC3::GetFlagEC3ExtensionTypeA() const { return impl->extension_type_a; }
uint8_t DEC3::GetComplexityIndexTypeA() const {
  return impl->complexity_index_type_a;
}
std::vector<uint8_t> DEC3::GetAdditionalData() const {
  return impl->additional_data;
}
std::vector<std::pair<std::string, std::string> >
DEC3::GetDisplayableProperties() const {
  auto props = Box::GetDisplayableProperties();
  props.push_back({"Data Rate (kbit/s)", std::to_string(GetDataRate())});
  props.push_back(
      {"Independent Substreams", std::to_string(impl->substreams.size())});
  for (size_t i = 0; i < impl->substreams.size(); ++i) {
    const auto& s = impl->substreams[i];
    props.push_back({"Substream " + std::to_string(i),
                     "fscod=" + std::to_string(s.fscod) +
                         ", bsid=" + std::to_string(s.bsid) +
                         ", asvc=" + std::to_string(s.asvc) +
                         ", bsmod=" + std::to_string(s.bsmod) +
                         ", acmod=" + std::to_string(s.acmod) +
                         ", lfeon=" + std::to_string(s.lfeon) +
                         ", num_dep_sub=" + std::to_string(s.num_dep_sub) +
                         ", chan_loc=" + std::to_string(s.chan_loc)});
  }
  if (HasExtension()) {
    props.push_back(
        {"EC3 Extension Type A", GetFlagEC3ExtensionTypeA() ? "1" : "0"});
    if (GetFlagEC3ExtensionTypeA()) {
      props.push_back({"Complexity Index Type A",
                       std::to_string(GetComplexityIndexTypeA())});
    }
  }
  return props;
}
}  // namespace ISOBMFF

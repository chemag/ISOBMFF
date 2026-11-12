/*******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 DigiDNA - www.digidna.net
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

/*!
 * @file        Parser.cpp
 * @copyright   (c) 2017, DigiDNA - www.digidna.net
 * @author      Jean-David Gadina - www.digidna.net
 */

#include <AV01.hpp>
#include <AV1C.hpp>
#include <AV2CInBand.hpp>
#include <AV2COutOfBand.hpp>
#include <AV2SampleEntry.hpp>
#include <AVC1.hpp>
#include <AVC3.hpp>
#include <AVCC.hpp>
#include <BinaryDataStream.hpp>
#include <BinaryFileStream.hpp>
#include <CDSC.hpp>
#include <COLR.hpp>
#include <CTTS.hpp>
#include <ContainerBox.hpp>
#include <DEC3.hpp>
#include <DIMG.hpp>
#include <DOPS.hpp>
#include <DREF.hpp>
#include <EC3.hpp>
#include <ELST.hpp>
#include <FRMA.hpp>
#include <FTYP.hpp>
#include <HDLR.hpp>
#include <HEV1.hpp>
#include <HVC1.hpp>
#include <HVCC.hpp>
#include <IINF.hpp>
#include <ILOC.hpp>
#include <INFE.hpp>
#include <IPCO.hpp>
#include <IPMA.hpp>
#include <IREF.hpp>
#include <IROT.hpp>
#include <ISPE.hpp>
#include <MDHD.hpp>
#include <META.hpp>
#include <MP4A.hpp>
#include <MVHD.hpp>
#include <OPUS.hpp>
#include <PITM.hpp>
#include <PIXI.hpp>
#include <Parser.hpp>
#include <SCHM.hpp>
#include <STSD.hpp>
#include <STSS.hpp>
#include <STSZ.hpp>
#include <STTS.hpp>
#include <STZ2.hpp>
#include <THMB.hpp>
#include <TKHD.hpp>
#include <URL.hpp>
#include <URN.hpp>
#include <cstring>
#include <map>
#include <stdexcept>
#include <utility>

namespace ISOBMFF {
class Parser::IMPL {
 public:
  IMPL();
  IMPL(const IMPL& o);
  ~IMPL();

  Error RegisterBox(const std::string& type,
                    const std::function<std::shared_ptr<Box>()>& createBox);
  Error RegisterContainerBox(const std::string& type);

  std::shared_ptr<File> _file;
  std::string _path;
  // The boxes registered on this parser (RegisterBox()), which win over
  // the default ones (DefaultBoxes()).
  std::map<std::string, std::function<std::shared_ptr<Box>()> > _types;
  Parser::StringType _stringType;
  uint64_t _options;
  std::map<std::string, void*> _info;
};

namespace {
using BoxFactory = std::function<std::shared_ptr<Box>()>;
using BoxFactories = std::map<std::string, BoxFactory>;

// Collects the default box factories, for DefaultBoxes().
class DefaultBoxTable {
 public:
  void RegisterBox(const std::string& type, const BoxFactory& createBox) {
    this->_types[type] = createBox;
  }
  void RegisterContainerBox(const std::string& type) {
    this->RegisterBox(type, [type]() -> std::shared_ptr<Box> {
      return std::make_shared<ContainerBox>(type);
    });
  }

  BoxFactories _types;
};

const BoxFactories* BuildDefaultBoxes();

// The default box factories, shared by every Parser. They used to be
// registered again in every Parser (a map of about 70 entries, built and
// freed with each one), which dominated the cost of creating a Parser.
// The table is built once, on the first call (thread-safe since C++11),
// and never destroyed, so that a Parser used from the destructor of a
// static object at exit still finds it.
const BoxFactories& DefaultBoxes() {
  static const BoxFactories* const defaults = BuildDefaultBoxes();
  return *defaults;
}
}  // namespace

Parser::Parser() : impl(std::make_unique<IMPL>()) {}

Parser::Parser(const std::string& path) : impl(std::make_unique<IMPL>()) {
  this->Parse(path);
}

Parser::Parser(const std::vector<uint8_t>& data)
    : impl(std::make_unique<IMPL>()) {
  this->Parse(data);
}

Parser::Parser(BinaryStream& stream) : impl(std::make_unique<IMPL>()) {
  this->Parse(stream);
}

Parser::Parser(const Parser& o) : impl(std::make_unique<IMPL>(*(o.impl))) {}

Parser::Parser(Parser&& o) noexcept : impl(std::move(o.impl)) {
  o.impl = nullptr;
}

Parser::~Parser() {}

Parser& Parser::operator=(Parser o) {
  swap(*(this), o);

  return *(this);
}

void swap(Parser& o1, Parser& o2) {
  using std::swap;

  swap(o1.impl, o2.impl);
}

Error Parser::RegisterContainerBox(const std::string& type) {
  return this->impl->RegisterContainerBox(type);
}

Error Parser::RegisterBox(
    const std::string& type,
    const std::function<std::shared_ptr<Box>()>& createBox) {
  return this->impl->RegisterBox(type, createBox);
}

std::shared_ptr<Box> Parser::CreateBox(const std::string& type) const {
  // A box registered on this parser wins over the default one. A null
  // factory registered on this parser turns the default off: the box is
  // read as a generic Box.
  const auto own = this->impl->_types.find(type);
  if (own != this->impl->_types.end()) {
    if (own->second == nullptr) {
      return std::make_shared<Box>(type);
    }
    return own->second();
  }

  const BoxFactories& defaults = DefaultBoxes();
  const auto def = defaults.find(type);
  if (def != defaults.end()) {
    return def->second();
  }

  return std::make_shared<Box>(type);
}

Error Parser::Parse(const std::string& path) {
  BinaryFileStream stream(path);

  Error err = this->Parse(stream);
  if (err) return err;

  this->impl->_path = path;

  return Error();
}

Error Parser::Parse(const std::vector<uint8_t>& data) {
  BinaryDataStream stream(data);

  return this->Parse(stream);
}

Error Parser::Parse(BinaryStream& stream) {
  char n[4] = {0, 0, 0, 0};

  if (stream.HasBytesAvailable() == false) {
    return Error(ErrorCode::CannotReadFile, "Cannot read file");
  }

  Error err = stream.Get(reinterpret_cast<uint8_t*>(n), 4, 4);
  if (err) {
    return Error(ErrorCode::CannotReadFile, "Cannot read file header");
  }

  if (memcmp(n, "ftyp", 4) != 0 && memcmp(n, "sinf", 4) != 0 &&
      memcmp(n, "wide", 4) != 0 && memcmp(n, "free", 4) != 0 &&
      memcmp(n, "skip", 4) != 0 && memcmp(n, "mdat", 4) != 0 &&
      memcmp(n, "moov", 4) != 0 && memcmp(n, "pnot", 4) != 0) {
    return Error(ErrorCode::NotISOMediaFile, "Data is not an ISO media file");
  }

  this->impl->_path = "";
  this->impl->_file = std::make_shared<File>();

  if (stream.HasBytesAvailable()) {
    err = this->impl->_file->ReadData(*(this), stream);
    if (err) return err;
  }

  return Error();
}

std::shared_ptr<File> Parser::GetFile() const { return this->impl->_file; }

Parser::StringType Parser::GetPreferredStringType() const {
  return this->impl->_stringType;
}

void Parser::SetPreferredStringType(StringType value) {
  this->impl->_stringType = value;
}

uint64_t Parser::GetOptions() const { return this->impl->_options; }

void Parser::SetOptions(uint64_t value) { this->impl->_options = value; }

void Parser::AddOption(Options option) {
  this->impl->_options |= static_cast<uint64_t>(option);
}

void Parser::RemoveOption(Options option) {
  this->impl->_options &= ~static_cast<uint64_t>(option);
}

bool Parser::HasOption(Options option) {
  return (this->GetOptions() & static_cast<uint64_t>(option)) != 0;
}

const void* Parser::GetInfo(const std::string& key) {
  if (this->impl->_info.find(key) == this->impl->_info.end()) {
    return nullptr;
  }

  return this->impl->_info[key];
}

void Parser::SetInfo(const std::string& key, void* value) {
  if (value == nullptr) {
    this->impl->_info.erase(key);
  } else {
    this->impl->_info[key] = value;
  }
}

Parser::IMPL::IMPL()
    : _stringType(Parser::StringType::NULLTerminated), _options(0) {}

Parser::IMPL::IMPL(const IMPL& o)
    : _file(o._file),
      _path(o._path),
      _types(o._types),
      _stringType(o._stringType),
      _options(o._options),
      _info(o._info) {}

Parser::IMPL::~IMPL() {}

Error Parser::IMPL::RegisterBox(
    const std::string& type,
    const std::function<std::shared_ptr<Box>()>& createBox) {
  if (type.size() != 4) {
    return Error(ErrorCode::InvalidBoxData,
                 "Box name should be 4 characters long");
  }

  this->_types[type] = createBox;
  return Error();
}

Error Parser::IMPL::RegisterContainerBox(const std::string& type) {
  return this->RegisterBox(type, [=]() -> std::shared_ptr<Box> {
    return std::make_shared<ContainerBox>(type);
  });
}

namespace {
const BoxFactories* BuildDefaultBoxes() {
  DefaultBoxTable table;

  table.RegisterContainerBox("moov");
  table.RegisterContainerBox("trak");
  table.RegisterContainerBox("edts");
  table.RegisterContainerBox("mdia");
  table.RegisterContainerBox("minf");
  table.RegisterContainerBox("stbl");
  table.RegisterContainerBox("mvex");
  table.RegisterContainerBox("moof");
  table.RegisterContainerBox("traf");
  table.RegisterContainerBox("mfra");
  table.RegisterContainerBox("meco");
  table.RegisterContainerBox("mere");
  table.RegisterContainerBox("dinf");
  table.RegisterContainerBox("ipro");
  table.RegisterContainerBox("sinf");
  table.RegisterContainerBox("iprp");
  table.RegisterContainerBox("fiin");
  table.RegisterContainerBox("paen");
  table.RegisterContainerBox("strk");
  table.RegisterContainerBox("tapt");
  table.RegisterContainerBox("schi");

  table.RegisterBox("ftyp", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<FTYP>();
  });
  table.RegisterBox("mvhd", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<MVHD>();
  });
  table.RegisterBox("tkhd", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<TKHD>();
  });
  table.RegisterBox("meta", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<META>();
  });
  table.RegisterBox("hdlr", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<HDLR>();
  });
  table.RegisterBox("mdhd", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<MDHD>();
  });
  table.RegisterBox("pitm", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<PITM>();
  });
  table.RegisterBox("iinf", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<IINF>();
  });
  table.RegisterBox("dref", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<DREF>();
  });
  table.RegisterBox("elst", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<ELST>();
  });
  table.RegisterBox("url ", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<URL>();
  });
  table.RegisterBox("urn ", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<URN>();
  });
  table.RegisterBox("iloc", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<ILOC>();
  });
  table.RegisterBox("iref", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<IREF>();
  });
  table.RegisterBox("infe", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<INFE>();
  });
  table.RegisterBox("irot", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<IROT>();
  });
  table.RegisterBox("hvcC", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<HVCC>();
  });
  table.RegisterBox("avcC", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<AVCC>();
  });
  table.RegisterBox("dimg", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<DIMG>();
  });
  table.RegisterBox("thmb", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<THMB>();
  });
  table.RegisterBox("cdsc", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<CDSC>();
  });
  table.RegisterBox("colr", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<COLR>();
  });
  table.RegisterBox("ispe", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<ISPE>();
  });
  table.RegisterBox("ipma", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<IPMA>();
  });
  table.RegisterBox("pixi", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<PIXI>();
  });
  table.RegisterBox("ipco", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<IPCO>();
  });
  table.RegisterBox("stsd", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<STSD>();
  });
  table.RegisterBox("stss", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<STSS>();
  });
  table.RegisterBox("stsz", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<STSZ>();
  });
  table.RegisterBox("stz2", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<STZ2>();
  });
  table.RegisterBox("stts", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<STTS>();
  });
  table.RegisterBox("ctts", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<CTTS>();
  });
  table.RegisterBox("frma", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<FRMA>();
  });
  table.RegisterBox("schm", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<SCHM>();
  });
  table.RegisterBox("hvc1", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<HVC1>();
  });
  table.RegisterBox("hev1", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<HEV1>();
  });
  table.RegisterBox("avc1", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<AVC1>();
  });
  table.RegisterBox("avc3", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<AVC3>();
  });
  table.RegisterBox("av01", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<AV01>();
  });
  table.RegisterBox("av1C", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<AV1C>();
  });
  table.RegisterBox("av2E", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<AV2SampleEntry>("av2E");
  });
  table.RegisterBox("av2e", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<AV2SampleEntry>("av2e");
  });
  table.RegisterBox("av2C", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<AV2COutOfBand>();
  });
  table.RegisterBox("av2c", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<AV2CInBand>();
  });
  table.RegisterBox("ec-3", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<EC3>();
  });
  table.RegisterBox("dec3", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<DEC3>();
  });
  table.RegisterBox("mp4a", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<MP4A>();
  });
  table.RegisterBox("Opus", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<OPUS>();
  });
  table.RegisterBox("dOps", [=]() -> std::shared_ptr<Box> {
    return std::make_shared<DOPS>();
  });

  return new BoxFactories(std::move(table._types));
}
}  // namespace
}  // namespace ISOBMFF

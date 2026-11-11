# ISOBMFF Library Analysis

## Overview

ISOBMFF is a cross-platform C++ library for reading and parsing files in the **ISO Base Media File Format** (ISO/IEC 14496-12). The library is designed to handle various container formats built on this standard, including QuickTime (.mov), MPEG-4 (.m4v, .m4a), 3GP (.3g2), and HEIF/HEIC (.heif, .heic).

**Current Version:** 0.4
**License:** MIT
**Language:** C++14
**Original Author:** DigiDNA (www.digidna.net)

## Purpose

The library provides a comprehensive parser for ISO base media file formats, which are container formats used for time-based multimedia files such as video and audio. The format is designed to be:
- Flexible and extensible
- Independent of network protocols while enabling support for them
- Suitable for interchange, management, editing, and presentation of media

**Note:** The library currently supports **reading/parsing only**. Writing/editing support is not yet available, but the architecture is designed with this future capability in mind.

## Architecture

### Core Components

1. **Parser (`Parser.hpp`)** - Main parsing engine
   - Parses files, data buffers, or streams
   - Supports custom box registration
   - Configurable string types (NULL-terminated UTF-8 or Pascal strings)
   - Parser options (e.g., `DoNotSkipMDATData` to keep media data)
   - Contextual info storage for parsing state

2. **File (`File.hpp`)** - Represents parsed ISO media files
   - Inherits from `ContainerBox`
   - Acts as the root container for all boxes

3. **Box System** - Hierarchical box/atom structure
   - `Box` - Base class for all box types
   - `FullBox` - Extended box with version and flags
   - `ContainerBox` - Boxes that contain other boxes
   - 54+ specialized box implementations (FTYP, MVHD, TKHD, HDLR, STSD, etc.)

4. **Stream System** - Binary data reading
   - `BinaryStream` - Abstract base class
   - `BinaryFileStream` - File-based reading
   - `BinaryDataStream` - Memory buffer reading

### Supported Box Types

The library implements parsers for 60+ box types, including:

**Container Boxes:** MOOV, TRAK, MDIA, MINF, DINF, STBL, META, etc.

**Media Boxes:**
- Video: AVC1, HVC1, AV01 (H.264, HEVC, AV1)
- Audio: MP4A
- Configuration: AVCC, HVCC (codec configuration)

**Metadata Boxes:** FTYP, MVHD, TKHD, MDHD, HDLR, COLR

**Sample Tables:** STSD, STTS, STSS, STSC, STSZ, STCO, CTTS

**HEIF/HEIC:** IINF, INFE, ILOC, IREF, IPCO, IPMA, ISPE, IROT, DIMG

**Data References:** DREF, URL

## Key Features

### 1. Extensible Box Registration
The parser allows custom box types to be registered dynamically:
```cpp
ISOBMFF::Error error = parser.RegisterBox("abcd", []() -> std::shared_ptr<CustomBox> {
    return std::make_shared<CustomBox>();
});
if (error) {
  // Handle registration error
  std::cerr << "Box registration failed: " << error.GetMessage() << std::endl;
}
```

### 2. Type-Safe Box Retrieval
Boxes can be retrieved with type checking:
```cpp
std::shared_ptr<ISOBMFF::FTYP> ftyp = file->GetTypedBox<ISOBMFF::FTYP>("ftyp");
```

### 3. Multiple Input Methods
- File path parsing
- In-memory buffer parsing
- Stream-based parsing

### 4. Display Support
All boxes implement `GetDisplayableProperties()` for inspection and debugging.

## Project Structure

```
isobmff/
+-- include/          # 54 header files (public API)
+-- src/              # 61 implementation files
+-- tools/            # Command-line dump utility (main.cpp)
+-- test/             # 21 unit tests using Google Test framework
+-- fuzz/             # 23 fuzzing targets (libFuzzer integration)
   +-- converter.py   # Auto-generates fuzzers from unit tests
   +-- corpus/        # Fuzzing seed corpus
+-- build/            # Build output directory
+-- VisualStudio/     # Windows build support (VS 2019/2022)
```

## Build System

**Supported Platforms:**
- macOS (Xcode project + Makefile)
- Linux/Unix (Makefile + CMake)
- Windows (Visual Studio 2019/2022 solutions)

**Build Targets:**
- Static library (ISOBMFF)
- Dynamic library/DLL (Windows)
- Command-line dump utility (ISOBMFF-Dump)

**Build Options (CMakeLists.txt):**
- `BUILD_TESTS` (default: ON) - Build unit tests
- `BUILD_CLANG_FUZZER` (default: ON) - Build fuzzing targets

**Compiler Requirements:**
- C++14 standard
- Clang or GCC on Linux
- Apple Clang on macOS
- MSVC v142/v143 on Windows

## Tools

### ISOBMFF-Dump Utility
Command-line tool for analyzing ISO media files (`tools/main.cpp`):

**Features:**
- Parse and display structure of media files
- Debug output with `-d` flag (repeatable for more verbosity)
- Quiet mode with `-q` or `--quiet`
- Optional analysis output with `--analyze-flag` (default) or `--no-analyze-flag`
- Supports multiple input files

**Usage:**
```bash
./ISOBMFF-Dump [-d] [-q] [--analyze-flag] file1.mp4 file2.mov ...
```

## Testing & Quality Assurance

### Unit Tests (`test/`)
- 21 unit test files covering major box types
- Uses Google Test framework
- Tests for: Parser, FTYP, MVHD, TKHD, HDLR, MDHD, STSD, STTS, STSS, CTTS, DREF, URL, META, AVC1, AVCC, HVC1, HVCC, MP4A

### Fuzzing (`fuzz/`)
- 23 fuzzing targets for libFuzzer/AFL
- Auto-generation infrastructure using `converter.py`
- Converts unit tests to fuzzers with special annotations:
  - `// fuzzer::conv: begin` ... `// fuzzer::conv: end` - Marks code to fuzz
  - `// fuzzer::conv: data` - Marks buffer data for corpus generation
- Corpus-based fuzzing with seed inputs from unit tests
- Recent fuzzing results: Successfully completed 10^7 (10 million) fuzzing iterations on AVCC and AVC1 atom parsers

### Continuous Integration
- GitHub Actions workflows for macOS and Windows builds
- Build status badges in README

## Usage Example

```cpp
#include <ISOBMFF.hpp>

// Create parser
ISOBMFF::Parser parser;

// Parse file (returns Error code)
ISOBMFF::Error error = parser.Parse("video.mp4");
if (error) {
  std::cerr << "Parse error: " << error.GetMessage() << std::endl;
  return -1;
}

// Get file object
std::shared_ptr<ISOBMFF::File> file = parser.GetFile();

// Retrieve file type box
std::shared_ptr<ISOBMFF::FTYP> ftyp = file->GetTypedBox<ISOBMFF::FTYP>("ftyp");

// Navigate container hierarchy
std::shared_ptr<ISOBMFF::ContainerBox> moov = file->GetTypedBox<ISOBMFF::ContainerBox>("moov");
std::shared_ptr<ISOBMFF::MVHD> mvhd = moov->GetTypedBox<ISOBMFF::MVHD>("mvhd");

// Display file structure
std::cout << *file << std::endl;
```

## Recent Development Activity

Based on git history:
- **v0.4** - August 15, 2025
- Recent focus on fuzzing infrastructure (10^7 iterations on AVCC/AVC1 parsers)
- Unit test additions for AVCC atom parser
- Active development on video codec support (H.264/AVC, HEVC)

## Notable Design Patterns

1. **PIMPL (Pointer to Implementation)** - All major classes use the PIMPL idiom with `class IMPL` to hide implementation details
2. **Factory Pattern** - Box creation via registered factory functions
3. **Type-safe casting** - Custom casting utilities (`Casts.hpp`)
4. **Stream abstraction** - Unified interface for file and memory-based parsing
5. **Displayable objects** - Consistent interface for debugging and inspection via `DisplayableObject` and `DisplayableObjectContainer`
6. **Error Code Pattern** - All functions return `ISOBMFF::Error`, with output parameters passed by reference. It does not use exceptions

## Error Handling Design

The library has been refactored to use **error codes instead of exceptions**. Key changes:

### API Pattern Changes:
- **OLD**: `value = stream.ReadUInt8();` (throws on error)
- **NEW**: `Error err = stream.ReadUInt8(value);` (returns error, value as output parameter)

### Function Signatures:
```cpp
// BinaryStream read functions
Error ReadUInt8(uint8_t& value);
Error ReadBigEndianUInt16(uint16_t& value);
Error Read(std::vector<uint8_t>& data, size_t size);

// Parser functions
Error Parse(const std::string& path);
Error RegisterBox(const std::string& type, const std::function<std::shared_ptr<Box>()>& createBox);

// Box ReadData functions
Error ReadData(Parser& parser, BinaryStream& stream) override;
```

### Benefits:
- **No exception handling required** - simpler error management
- **Consistent error reporting** - all functions use the same pattern
- **Better debugging** - explicit error checking at each step
- **Performance** - no exception overhead

## Limitations

- **Read-only** - No file writing/editing support yet
- **ISO format focus** - Primarily designed for ISO base media file formats
- **Memory-based parsing** - Full file is parsed into memory structures

## Dependencies

- C++ Standard Library (C++14)
- getopt for command-line parsing (POSIX)
- Google Test framework (for unit tests)
- libFuzzer (for fuzzing, optional)

## Summary

ISOBMFF is a well-architected, production-quality library for parsing ISO base media files. It features comprehensive box type support, extensibility for custom formats, robust testing infrastructure including fuzzing, and cross-platform compatibility. The library is actively maintained and follows modern C++ practices with clear separation of concerns and type safety.

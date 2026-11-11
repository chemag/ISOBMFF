# isobmff: A Library and Tool to parse ISO Base Media Files

[![ci-linux](https://img.shields.io/github/actions/workflow/status/chemag/ISOBMFF/ci-linux.yaml?label=linux&logo=linux)](https://github.com/chemag/ISOBMFF/actions/workflows/ci-linux.yaml)
[![ci-macos](https://img.shields.io/github/actions/workflow/status/chemag/ISOBMFF/ci-macos.yaml?label=macos&logo=apple)](https://github.com/chemag/ISOBMFF/actions/workflows/ci-macos.yaml)
[![ci-windows](https://img.shields.io/github/actions/workflow/status/chemag/ISOBMFF/ci-windows.yaml?label=windows&logo=windows)](https://github.com/chemag/ISOBMFF/actions/workflows/ci-windows.yaml)


# 1. Rationale

isobmff is a cross-platform C++ library, and a command-line tool, to read
and parse files in the ISO Base Media File Format (ISO/IEC 14496-12, MPEG-4
Part 12).

The ISO Base Media File Format is a container format, used by many file
types, including:

* QuickTime (`.mov`)
* MPEG-4 (`.mp4`, `.m4v`, `.m4a`, etc.)
* 3GP (`.3gp`, `.3g2`)
* HEIF/HEIC (`.heif`, `.heic`, etc.)

> ISO base media file format (ISO/IEC 14496-12 - MPEG-4 Part 12) defines a
> general structure for time-based multimedia files such as video and audio.
> It is designed as a flexible, extensible format that facilitates
> interchange, management, editing and presentation of the media. The
> presentation may be local, or via a network or other stream delivery
> mechanism. The file format is designed to be independent of any particular
> network protocol while enabling support for them in general. It is used as
> the basis for other media file formats (e.g. container formats MP4 and
> 3GP).

Source: [Wikipedia](https://en.wikipedia.org/wiki/ISO_base_media_file_format)

The library parses files; it does not write or edit them.


# 2. Install Instructions

Get the git repo, and then build using cmake.

```
$ git clone https://github.com/chemag/isobmff
$ cd isobmff
$ cmake -S . -B build -DBUILD_CLANG_FUZZER=OFF
$ cmake --build build
```

This builds the library, the `isobmff` tool, and the unit tests, on Linux
(gcc or clang), macOS (Apple Clang), and Windows (MSVC). With MSVC, the tool
is not built, since it uses `getopt_long()`.

Some cmake options:
* "`cmake -DCMAKE_BUILD_TYPE=Debug`": build with the strict warning flags in
  `CMakeLists.txt`, which treat warnings as errors (gcc and clang)
* "`cmake -DBUILD_TESTS=OFF`": do not build the tests
* "`cmake -DBUILD_CLANG_FUZZER=OFF`": do not build the fuzzers (they need
  clang)

`make check-debug` runs Debug builds with gcc and clang, and the tests. The
`.github/workflows/` files show the exact Linux, macOS, and Windows setups.

Feel free to run all the unittests:

```
$ ctest --test-dir build
```

The `box_coverage` test checks that every box the parser registers has a
unit test and a fuzzer.


# 3. CLI Binary Operation

The `isobmff` tool dumps the structure of one or more files:

```
$ ./build/tools/isobmff media/MOV1.MOV
```

Options:
* "`-d`": increase debug verbosity
* "`-q`": zero debug verbosity
* "`--analyze-flag`", "`--no-analyze-flag`": set or reset the analyze flag
* "`-h`": help


# 4. Programmatic Integration Operation

Parsing is done with the `ISOBMFF::Parser` class:

```cpp
ISOBMFF::Parser parser;

ISOBMFF::Error error = parser.Parse("path/to/some/file");
if (error) {
  // Handle error
  std::cerr << "Error: " << error.GetMessage() << std::endl;
}
```

When the parser is done, an instance of `ISOBMFF::File` can be retrieved:

```cpp
std::shared_ptr<ISOBMFF::File> file = parser.GetFile();
```

Boxes can then be retrieved the following way, from the file:

```cpp
std::shared_ptr<ISOBMFF::Box> box = file->GetBox("ftyp");
```

If the box does not exist, it will return `nullptr`.

A typed box can be retrieved the following way:

```cpp
std::shared_ptr<ISOBMFF::FTYP> ftyp = file->GetTypedBox<ISOBMFF::FTYP>("ftyp");
```

Here, `nullptr` will be returned if the box does not exist, or is not of the
correct type.

Container boxes act just the same:

```cpp
std::shared_ptr<ISOBMFF::ContainerBox> moov = file->GetTypedBox<ISOBMFF::ContainerBox>("moov");
std::shared_ptr<ISOBMFF::MVHD> mvhd = moov->GetTypedBox<ISOBMFF::MVHD>("mvhd");
```

The parser also supports custom boxes:

```cpp
ISOBMFF::Error error = parser.RegisterBox("abcd", [=]() -> std::shared_ptr<CustomBox> {
  return std::make_shared<CustomBox>();
});
if (error) {
  // Handle registration error
  std::cerr << "Box registration error: " << error.GetMessage() << std::endl;
}
```

It will then create an instance of `CustomBox` when encountering an `abcd`
box somewhere in the file.

The custom class needs at least to inherit from `Box`:

```cpp
class CustomBox : public ISOBMFF::Box {
 public:
  CustomBox() : Box("abcd") {}

  ISOBMFF::Error ReadData(ISOBMFF::Parser& parser,
                          ISOBMFF::BinaryStream& stream) override {
    // Read box data here, and return Error() on success or the actual error
    return ISOBMFF::Error();
  }

  std::vector<std::pair<std::string, std::string>> GetDisplayableProperties()
      const override {
    // Return the box properties, to support output
    return {};
  }
};
```


# 5. Requirements

Requires googletest (gtest and gmock) for the unit tests.

Requires clang (with libfuzzer) for the fuzzers.


# 6. Other

The [fuzz](fuzz/README.md) directory contains information on fuzzing the
parser. The fuzzers are generated from the unit tests.

The [TODO](TODO.md) file tracks known gaps.


# 7. License

isobmff is MIT licensed, as found in the [LICENSE](LICENSE) file.

This is a fork of [ISOBMFF](https://github.com/DigiDNA/ISOBMFF).

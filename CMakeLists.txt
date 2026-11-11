cmake_minimum_required(VERSION 3.12)

# Define the project name and version
project(ISOBMFF VERSION 0.17)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)

# Verify the compiler
if(APPLE)
    if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        message(FATAL_ERROR "Apple Clang is required on macOS.")
    endif()
elseif(UNIX AND NOT APPLE)
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        message(FATAL_ERROR "This project requires either Clang or GNU as the C++ compiler on Linux.")
    endif()
endif()

# Set debug flags for both C and C++ with gcc and clang (MSVC keeps its own
# defaults). Some of the warnings exist only in clang (or only in newer gcc
# releases), and gcc rejects an unknown -W option, so each of those is added
# only if the compiler accepts it.
if(NOT MSVC)
  set(COMMON_DEBUG_FLAGS "-g -O0 -Wall -Wextra -Wunused-parameter -Wshadow -Wformat -Wextra-semi -Wsign-conversion -Wold-style-cast -Wfloat-conversion -Wdouble-promotion -Wcast-qual -Wcast-align -Wno-covered-switch-default -Werror")
  include(CheckCXXCompilerFlag)
  foreach(flag
      -Wtautological-unsigned-zero-compare
      -Wshorten-64-to-32
      -Wimplicit-int-conversion
      -Wimplicit-float-conversion
      -Wmissing-variable-declarations
      -Winvalid-utf8
      -Wunreachable-code-break
      -Wextra-semi-stmt)
    string(MAKE_C_IDENTIFIER "HAVE_CXX_FLAG${flag}" have_flag)
    check_cxx_compiler_flag(${flag} ${have_flag})
    if(${have_flag})
      set(COMMON_DEBUG_FLAGS "${COMMON_DEBUG_FLAGS} ${flag}")
    endif()
  endforeach()

  set(CMAKE_C_FLAGS_DEBUG "${COMMON_DEBUG_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${COMMON_DEBUG_FLAGS}")
endif()


# Add subdirectories for tests and fuzzers if enabled
add_subdirectory(src)
# The tool uses getopt_long(), which MSVC does not provide.
if(NOT MSVC)
  add_subdirectory(tools)
endif()

option(BUILD_TESTS "Build ISOBMFF tests" ON)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

option(BUILD_CLANG_FUZZER "Build clang fuzzer sanitizer targets" ON)
if(BUILD_CLANG_FUZZER)
  add_subdirectory(fuzz)
endif()

# Copyright (c) Meta Platforms, Inc. and its affiliates.
#
# Box coverage check, run as a CTest (box_coverage): every box registered in
# src/Parser.cpp (RegisterBox("xxxx", ...)) must be created by a unit test
# (CreateBox("xxxx") in test/*_unittest.cpp) and by a fuzzer (CreateBox("xxxx")
# in fuzz/*_fuzzer.cpp, generated from the unit tests by fuzz/converter.py).
#
# Usage: cmake -DSOURCE_DIR=<isobmff source dir> -P check_box_coverage.cmake

cmake_minimum_required(VERSION 3.12)

if(NOT SOURCE_DIR)
  message(FATAL_ERROR "SOURCE_DIR is not set")
endif()

function(collect_fourccs out_var regex)
  set(fourccs "")
  foreach(file ${ARGN})
    file(READ "${file}" content)
    string(REGEX MATCHALL "${regex}\\(\"[^\"]+\"" matches "${content}")
    foreach(match ${matches})
      string(REGEX REPLACE "^.*\\(\"([^\"]+)\"$" "\\1" fourcc "${match}")
      list(APPEND fourccs "${fourcc}")
    endforeach()
  endforeach()
  list(REMOVE_DUPLICATES fourccs)
  set(${out_var} "${fourccs}" PARENT_SCOPE)
endfunction()

file(GLOB test_files "${SOURCE_DIR}/test/*_unittest.cpp")
file(GLOB fuzzer_files "${SOURCE_DIR}/fuzz/*_fuzzer.cpp")

collect_fourccs(registered "RegisterBox" "${SOURCE_DIR}/src/Parser.cpp")
collect_fourccs(tested "CreateBox" ${test_files})
collect_fourccs(fuzzed "CreateBox" ${fuzzer_files})

list(LENGTH registered num_registered)
if(num_registered EQUAL 0)
  message(FATAL_ERROR "no RegisterBox() calls found in src/Parser.cpp")
endif()

set(missing "")
foreach(fourcc ${registered})
  set(gaps "")
  if(NOT fourcc IN_LIST tested)
    list(APPEND gaps "unit test")
  endif()
  if(NOT fourcc IN_LIST fuzzed)
    list(APPEND gaps "fuzzer")
  endif()
  if(gaps)
    string(REPLACE ";" ", " gaps "${gaps}")
    list(APPEND missing "'${fourcc}' (no ${gaps})")
  endif()
endforeach()

if(missing)
  string(REPLACE ";" "\n  " missing "${missing}")
  message(FATAL_ERROR
    "registered boxes without a unit test or fuzzer creating them:\n"
    "  ${missing}\n"
    "Add a unit test with fuzzer::conv markers, then run fuzz/converter.py.")
endif()

message(STATUS "box coverage: all ${num_registered} registered boxes have a "
               "unit test and a fuzzer")

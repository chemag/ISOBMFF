/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

// This file was auto-generated using fuzz/converter.py from
// HVC1_unittest.cpp.
// Do not edit directly.

#include <ISOBMFF.hpp>                  // for various
#include <BinaryDataStream.hpp> // for BinaryDataStream
#include <Parser.hpp>           // for Parser


// libfuzzer infra to test the fuzz target
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  const std::vector<uint8_t> buffer_vector = {data, data + size};
  {
  ISOBMFF::BinaryDataStream stream(buffer_vector);
  ISOBMFF::Parser parser;
  std::shared_ptr<ISOBMFF::Box> box = parser.CreateBox("hvc1");
  ISOBMFF::Error error;
  if (box != nullptr) {
    error = box->ReadData(parser, stream);
  }
  if (error) {
    fprintf(stderr, "Parse error: %s\n", error.GetMessage().c_str());
  }
  }
  return 0;
}

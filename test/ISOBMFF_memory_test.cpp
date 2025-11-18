/*
 *  Copyright (c) Meta Platforms, Inc. and its affiliates.
 */

/*
 * Memory usage test for ISOBMFF library, specifically testing the
 * DoNotSkipMDATData option.
 *
 * This test demonstrates the memory impact of loading MDAT (Media Data)
 * boxes into memory vs. skipping them during parsing.
 *
 * MDAT boxes contain the actual video/audio samples and can be very large
 * (often hundreds of MB to GBs). By default, the parser skips MDAT data to
 * conserve memory. When DoNotSkipMDATData option is set, the parser loads
 * all MDAT data into memory.
 *
 * Test cases:
 * 1. MDATSkippingMemoryUsage: Verifies memory stays low when MDAT is skipped
 * (default behavior)
 * 2. MDATNotSkippingMemoryUsage: Shows high memory when MDAT is loaded
 *    (this test is EXPECTED TO FAIL for large files)
 *
 * Usage:
 *   # Run all tests
 *   ./isobmff_memory_test
 *
 *   # Run specific test
 *   ./isobmff_memory_test --gtest_filter=IsobmffMemoryTest.MDATSkippingMemoryUsage
 */

#include <gtest/gtest.h>

#include <Parser.hpp>
#include <iostream>
#include <string>

#if defined(__APPLE__) && defined(__MACH__)
#include <mach/mach.h>
#elif defined(__linux__)
#include <fstream>
#include <string>
#endif

namespace ISOBMFF {

// Test fixture for ISOBMFF memory usage tests
class IsobmffMemoryTest : public ::testing::Test {
 public:
  IsobmffMemoryTest() {}
  ~IsobmffMemoryTest() override {}

  // Gets current memory usage (RSS - Resident Set Size) in bytes
  size_t getCurrentMemoryUsageBytes() {
#if defined(__APPLE__) && defined(__MACH__)
    // macOS: Use Mach task_info API
    struct mach_task_basic_info info;
    mach_msg_type_number_t info_count = MACH_TASK_BASIC_INFO_COUNT;

    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info,
                  &info_count) != KERN_SUCCESS) {
      return 0;
    }
    return info.resident_size;
#elif defined(__linux__)
    // Linux: Read RSS from /proc/self/statm
    long rss = 0L;
    FILE* fp = nullptr;
    if ((fp = fopen("/proc/self/statm", "r")) == nullptr) {
      return 0;
    }
    if (fscanf(fp, "%*s%ld", &rss) != 1) {
      fclose(fp);
      return 0;
    }
    fclose(fp);
    return rss * sysconf(_SC_PAGESIZE);
#else
    return 0;
#endif
  }

  // Converts bytes to megabytes
  double bytesToMB(size_t bytes) {
    return static_cast<double>(bytes) / (1024.0 * 1024.0);
  }
};

// Test 1: Default behavior - MDAT data is skipped
// This test should SUCCEED because MDAT data is not loaded into memory
// Expected: Low memory usage (<50 MB increase)
TEST_F(IsobmffMemoryTest, MDATSkippingMemoryUsage) {
  std::string test_file = std::string(TEST_MEDIA_DIR) + "/MOV1.MOV";

  std::cout << "\n=== Test: MDAT Skipping (Default) ===" << std::endl;
  std::cout << "File: " << test_file << std::endl;

  size_t initial_memory = getCurrentMemoryUsageBytes();
  std::cout << "Initial memory: " << bytesToMB(initial_memory) << " MB"
            << std::endl;

  {
    // Create parser with default options (skip MDAT data)
    Parser parser(test_file);

    // Parse the file
    Error err = parser.Parse(test_file);
    ASSERT_TRUE(!err) << "Failed to parse file: " << err.GetMessage();

    auto file = parser.GetFile();
    ASSERT_NE(nullptr, file) << "Parsed file is null";

    size_t after_parse_memory = getCurrentMemoryUsageBytes();
    size_t memory_increase = after_parse_memory - initial_memory;

    std::cout << "After parsing: " << bytesToMB(after_parse_memory) << " MB"
              << std::endl;
    std::cout << "Memory increase: " << bytesToMB(memory_increase) << " MB"
              << std::endl;

    // With MDAT skipping, memory increase should be small
    // Just the metadata structures, not the actual video data
    double memory_threshold_mb = 50.0;  // Conservative threshold

    EXPECT_LT(bytesToMB(memory_increase), memory_threshold_mb)
        << "Memory increased by more than " << memory_threshold_mb
        << " MB when MDAT should be skipped";
  }

  size_t final_memory = getCurrentMemoryUsageBytes();
  std::cout << "Final memory: " << bytesToMB(final_memory) << " MB"
            << std::endl;
}

// Test 2: DoNotSkipMDATData option - MDAT data is loaded
// This test is EXPECTED TO FAIL for files larger than threshold
// because all MDAT data is loaded into memory
// Expected: High memory usage (proportional to file size)
TEST_F(IsobmffMemoryTest, MDATNotSkippingMemoryUsage) {
  std::string test_file = std::string(TEST_MEDIA_DIR) + "/MOV1.MOV";

  std::cout << "\n=== Test: MDAT Not Skipping (DoNotSkipMDATData) ==="
            << std::endl;
  std::cout << "File: " << test_file << std::endl;
  std::cout << "NOTE: This test is EXPECTED TO FAIL for large files"
            << std::endl;

  size_t initial_memory = getCurrentMemoryUsageBytes();
  std::cout << "Initial memory: " << bytesToMB(initial_memory) << " MB"
            << std::endl;

  {
    // Create parser and set DoNotSkipMDATData option
    Parser parser(test_file);
    parser.AddOption(Parser::Options::DoNotSkipMDATData);

    // Verify the option is set
    ASSERT_TRUE(parser.HasOption(Parser::Options::DoNotSkipMDATData))
        << "DoNotSkipMDATData option not set";

    // Parse the file
    Error err = parser.Parse(test_file);
    ASSERT_TRUE(!err) << "Failed to parse file: " << err.GetMessage();

    auto file = parser.GetFile();
    ASSERT_NE(nullptr, file) << "Parsed file is null";

    size_t after_parse_memory = getCurrentMemoryUsageBytes();
    size_t memory_increase = after_parse_memory - initial_memory;

    std::cout << "After parsing: " << bytesToMB(after_parse_memory) << " MB"
              << std::endl;
    std::cout << "Memory increase: " << bytesToMB(memory_increase) << " MB"
              << std::endl;

    // With DoNotSkipMDATData, memory should increase significantly
    // because all media data is loaded into memory
    //
    // This threshold is intentionally LOW to make the test fail
    // for typical video files, demonstrating the memory impact
    double memory_threshold_mb = 50.0;

    EXPECT_LT(bytesToMB(memory_increase), memory_threshold_mb)
        << "\n\n"
        << "========================================\n"
        << "EXPECTED FAILURE\n"
        << "========================================\n"
        << "Memory increased by " << bytesToMB(memory_increase) << " MB\n"
        << "This exceeds the threshold of " << memory_threshold_mb << " MB\n"
        << "\n"
        << "This is EXPECTED behavior when DoNotSkipMDATData is set.\n"
        << "The parser loaded all MDAT (media data) into memory.\n"
        << "\n"
        << "To fix: Remove DoNotSkipMDATData option to skip MDAT data.\n"
        << "See MDATSkippingMemoryUsage test for the correct behavior.\n"
        << "========================================\n";
  }

  size_t final_memory = getCurrentMemoryUsageBytes();
  std::cout << "Final memory: " << bytesToMB(final_memory) << " MB"
            << std::endl;
}

// Test 3: Comparison test - demonstrates the difference
// Parses the same file twice: once with skipping, once without
// Shows the memory difference side-by-side
TEST_F(IsobmffMemoryTest, MDATSkippingComparison) {
  std::string test_file = std::string(TEST_MEDIA_DIR) + "/MOV1.MOV";

  std::cout << "\n=== Test: MDAT Skipping Comparison ===" << std::endl;
  std::cout << "File: " << test_file << std::endl;

  size_t mem_with_skipping = 0;
  size_t mem_without_skipping = 0;

  // Test 1: With skipping (default)
  {
    size_t initial = getCurrentMemoryUsageBytes();
    Parser parser(test_file);
    Error err = parser.Parse(test_file);
    ASSERT_TRUE(!err);
    size_t after = getCurrentMemoryUsageBytes();
    mem_with_skipping = after - initial;
  }

  // Test 2: Without skipping (DoNotSkipMDATData)
  {
    size_t initial = getCurrentMemoryUsageBytes();
    Parser parser(test_file);
    parser.AddOption(Parser::Options::DoNotSkipMDATData);
    Error err = parser.Parse(test_file);
    ASSERT_TRUE(!err);
    size_t after = getCurrentMemoryUsageBytes();
    mem_without_skipping = after - initial;
  }

  std::cout << "\n=== Comparison Results ===" << std::endl;
  std::cout << "With MDAT skipping:    " << bytesToMB(mem_with_skipping)
            << " MB" << std::endl;
  std::cout << "Without MDAT skipping: " << bytesToMB(mem_without_skipping)
            << " MB" << std::endl;
  std::cout << "Difference:            "
            << bytesToMB(mem_without_skipping - mem_with_skipping) << " MB"
            << std::endl;

  // The difference should be significant (at least 10 MB for typical files)
  size_t difference = mem_without_skipping - mem_with_skipping;
  EXPECT_GT(bytesToMB(difference), 10.0)
      << "Expected significant memory difference between skipping/not skipping "
         "MDAT";
}
}  // namespace ISOBMFF

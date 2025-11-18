# Enable testing
enable_testing()
# Find GTest package
find_package(GTest REQUIRED)
# Include directories
include_directories(PUBLIC ../src)
# MacOS requirements
include_directories(PUBLIC /usr/local/include)
link_directories(/usr/local/lib)

# Define media directory for tests
set(TEST_MEDIA_DIR ${CMAKE_SOURCE_DIR}/media)

# Function to add a test executable
function(add_isobmff_test test_name)
  add_executable(${test_name} ${test_name}.cpp)

  # Add TEST_MEDIA_DIR compile definition
  target_compile_definitions(${test_name}
    PRIVATE TEST_MEDIA_DIR="${TEST_MEDIA_DIR}"
  )

  add_test(NAME ${test_name} COMMAND ${test_name})
  target_link_libraries(${test_name} PUBLIC isobmff GTest::gtest GTest::gtest_main)
endfunction()

# Find all test files in the current directory
file(GLOB TEST_FILES "*_unittest.cpp" "*_test.cpp")

# Extract test names from file names
foreach(test_file IN LISTS TEST_FILES)
  get_filename_component(test_name ${test_file} NAME_WE)
  add_isobmff_test(${test_name})
endforeach()

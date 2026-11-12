# Include directories
include_directories(PUBLIC ../include)
include_directories(PUBLIC ../src)

# MacOS requirements
include_directories(PUBLIC /usr/local/include)
link_directories(/usr/local/lib)

# The top level CMakeLists.txt instruments everything, including the
# library, with -fsanitize=address,undefined -fsanitize=fuzzer-no-link.
# Only the libFuzzer entry point is added here, where the targets are.
add_link_options(-fsanitize=fuzzer)

# Function to add a fuzzer executable
function(add_fuzzer TARGET SOURCE)
  if(NOT WIN32)
    add_executable(${TARGET} ${SOURCE})
    target_link_libraries(${TARGET} PUBLIC isobmff)
  endif()
endfunction()

# Find all fuzzer files in the current directory
file(GLOB FUZZER_FILES "*_fuzzer.cpp")
# Extract fuzzer names from file names and add them
foreach(fuzzer_file IN LISTS FUZZER_FILES)
  get_filename_component(fuzzer_name ${fuzzer_file} NAME_WE)
  add_fuzzer(${fuzzer_name} ${fuzzer_file})
endforeach()

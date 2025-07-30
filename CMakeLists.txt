# Set cmake version
cmake_minimum_required(VERSION 3.15)

# Project setup
project(logger VERSION 1.0 LANGUAGES CXX)

## Output Dirs
# Put binaries in bin/ and static libs in lib/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/")

## Logger Library
add_library(logger src/logger.cc)

# Make the 'include' dir available to our library and anything that uses it
target_include_directories(logger
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Require C++17 for this target
target_compile_features(logger PUBLIC cxx_std_17)

# Link against pthreads for std::thread
find_package(Threads REQUIRED)
target_link_libraries(logger PUBLIC Threads::Threads)


## Example App
# An executable for testing/demonstration
add_executable(example_logger examples/main.cc)

# Link our app against our logger library
target_link_libraries(example_logger PRIVATE logger)

cmake_minimum_required(VERSION 3.20)
project(fastfileparser)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add all source files
set(SOURCES
        index.cpp
        src/storage/mmap_manager.cpp
        src/io/csv_scanner.cpp
)

# Create executable
add_executable(fastfileparser ${SOURCES})

# Include directories for headers
target_include_directories(fastfileparser PRIVATE
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)
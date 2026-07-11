cmake_minimum_required(VERSION 3.20)
project(LoadTestingEngine
    VERSION 0.1.0
    LANGUAGES CXX)

# Set C++20 standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing
enable_testing()

# Set common include directories for the entire project
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Compile options for GCC/Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror)
endif()

# Recurse into subdirectories
add_subdirectory(src)
add_subdirectory(tests)

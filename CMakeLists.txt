cmake_minimum_required(VERSION 3.14)
project(internal_order_book VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(include)

# Main source
add_subdirectory(src)

# Tests
enable_testing()
add_subdirectory(test) 
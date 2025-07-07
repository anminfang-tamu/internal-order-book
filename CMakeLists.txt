cmake_minimum_required(VERSION 3.14)
project(internal_order_book VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

# Find Protobuf
find_package(Protobuf REQUIRED)

# Use pkg-config to find gRPC (more reliable than find_package)
pkg_check_modules(GRPC REQUIRED grpc++ grpc)

# Find protoc and grpc_cpp_plugin
find_program(PROTOC protoc)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin)

if(NOT PROTOC)
    message(FATAL_ERROR "protoc not found!")
endif()

if(NOT GRPC_CPP_PLUGIN)
    message(FATAL_ERROR "grpc_cpp_plugin not found!")
endif()

# Find Boost libraries
find_package(Boost REQUIRED COMPONENTS 
    system 
    filesystem 
    thread 
    chrono 
    date_time
    program_options
)

# Note: Boost.Lockfree is header-only, so no linking required

# Generate protobuf and gRPC files
set(PROTO_PATH "${CMAKE_CURRENT_SOURCE_DIR}/proto")
set(GENERATED_PROTOBUF_PATH "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GENERATED_PROTOBUF_PATH})

set(ORDERBOOK_PROTO "${PROTO_PATH}/orderbook_service.proto")
set(GENERATED_PROTOBUF_FILES
    "${GENERATED_PROTOBUF_PATH}/orderbook_service.pb.cc"
    "${GENERATED_PROTOBUF_PATH}/orderbook_service.pb.h"
    "${GENERATED_PROTOBUF_PATH}/orderbook_service.grpc.pb.cc"
    "${GENERATED_PROTOBUF_PATH}/orderbook_service.grpc.pb.h"
)

# Custom command to generate protobuf and gRPC files
add_custom_command(
    OUTPUT ${GENERATED_PROTOBUF_FILES}
    COMMAND ${PROTOC}
    ARGS --proto_path=${PROTO_PATH}
         --cpp_out=${GENERATED_PROTOBUF_PATH}
         --grpc_out=${GENERATED_PROTOBUF_PATH}
         --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}
         ${ORDERBOOK_PROTO}
    DEPENDS ${ORDERBOOK_PROTO}
    COMMENT "Generating protobuf and gRPC files"
)

# Create a library for generated protobuf files
add_library(orderbook_proto ${GENERATED_PROTOBUF_FILES})
target_link_libraries(orderbook_proto ${PROTOBUF_LIBRARIES} ${GRPC_LIBRARIES})
target_include_directories(orderbook_proto PUBLIC ${GENERATED_PROTOBUF_PATH} ${PROTOBUF_INCLUDE_DIRS} ${GRPC_INCLUDE_DIRS})
target_link_directories(orderbook_proto PUBLIC ${GRPC_LIBRARY_DIRS})

# Add include directories
include_directories(include)
include_directories(${GENERATED_PROTOBUF_PATH})

# Main source
add_subdirectory(src)

# Tests
enable_testing()
add_subdirectory(test) 
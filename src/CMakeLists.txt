# Original orderbook library
add_library(orderbook STATIC Order.cpp OrderBook.cpp MatchingEngine.cpp)
target_include_directories(orderbook PUBLIC ${CMAKE_SOURCE_DIR}/include)

# Link Boost libraries to the static library
target_link_libraries(orderbook PUBLIC 
    Boost::system
    Boost::filesystem
    Boost::thread
    Boost::chrono
    Boost::date_time
    Boost::program_options
)

# Original standalone executable
add_executable(internal-order-book
    main.cpp
    OrderBook.cpp
    Order.cpp
    MatchingEngine.cpp
)
target_include_directories(internal-order-book PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link Boost libraries to the executable
target_link_libraries(internal-order-book PRIVATE 
    Boost::system
    Boost::filesystem
    Boost::thread
    Boost::chrono
    Boost::date_time
    Boost::program_options
)

# gRPC Service Library
add_library(orderbook_grpc_service STATIC
    OrderBookServiceImpl.cpp
)

target_include_directories(orderbook_grpc_service PUBLIC 
    ${CMAKE_SOURCE_DIR}/include
    ${GENERATED_PROTOBUF_PATH}
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS}
)

target_link_libraries(orderbook_grpc_service PUBLIC
    orderbook
    orderbook_proto
    ${PROTOBUF_LIBRARIES}
    ${GRPC_LIBRARIES}
    Threads::Threads
)

target_link_directories(orderbook_grpc_service PUBLIC ${GRPC_LIBRARY_DIRS})

# gRPC Server Executable
add_executable(orderbook-grpc-server
    grpc_server_main.cpp
)

target_include_directories(orderbook-grpc-server PRIVATE 
    ${CMAKE_SOURCE_DIR}/include
    ${GENERATED_PROTOBUF_PATH}
    ${PROTOBUF_INCLUDE_DIRS}
    ${GRPC_INCLUDE_DIRS}
)

target_link_libraries(orderbook-grpc-server PRIVATE
    orderbook_grpc_service
    orderbook
    orderbook_proto
    ${PROTOBUF_LIBRARIES}
    ${GRPC_LIBRARIES}
    Threads::Threads
)

target_link_directories(orderbook-grpc-server PRIVATE ${GRPC_LIBRARY_DIRS})

# Ensure protobuf files are generated before building the gRPC components
add_dependencies(orderbook_grpc_service orderbook_proto)
add_dependencies(orderbook-grpc-server orderbook_proto) 
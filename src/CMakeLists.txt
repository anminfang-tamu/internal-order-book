add_executable(internal-order-book
    main.cpp
    order_book.cpp
)
target_include_directories(internal-order-book PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(orderbook STATIC main.cpp)

target_include_directories(orderbook PUBLIC ${CMAKE_SOURCE_DIR}/include) 
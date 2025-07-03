add_executable(internal-order-book
    main.cpp
    OrderBook.cpp
    Order.cpp
    MatchingEngine.cpp
)
target_include_directories(internal-order-book PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_library(orderbook STATIC Order.cpp OrderBook.cpp MatchingEngine.cpp)

target_include_directories(orderbook PUBLIC ${CMAKE_SOURCE_DIR}/include) 
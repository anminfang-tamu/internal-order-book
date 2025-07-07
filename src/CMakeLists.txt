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
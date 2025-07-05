cmake_minimum_required(VERSION 3.14)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Now add the test executable (includes all test files)
add_executable(general_tests 
    test_orderbook.cpp 
    test_order.cpp 
    test_matching_engine.cpp
    ../src/Order.cpp 
    ../src/OrderBook.cpp
    ../src/MatchingEngine.cpp
)
target_link_libraries(general_tests gtest_main)

# Add threading support for MatchingEngine tests
find_package(Threads REQUIRED)
target_link_libraries(general_tests Threads::Threads)

include(GoogleTest)
gtest_discover_tests(general_tests) 
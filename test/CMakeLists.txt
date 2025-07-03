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

# Now add the test executable
add_executable(orderbook_tests test_orderbook.cpp ../src/Order.cpp)
target_link_libraries(orderbook_tests gtest_main)

include(GoogleTest)
gtest_discover_tests(orderbook_tests) 
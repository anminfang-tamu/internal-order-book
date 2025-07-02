# Download and unpack GoogleTest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND 
  ${CMAKE_COMMAND} -G "Unix Makefiles" .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
execute_process(COMMAND 
  ${CMAKE_COMMAND} --build .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )

# Add googletest directly to our build. This defines the gtest and gtest_main targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
                 ${CMAKE_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# Now add the test executable
add_executable(orderbook_tests test_orderbook.cpp)
target_link_libraries(orderbook_tests orderbook gtest_main)

include(GoogleTest)
gtest_discover_tests(orderbook_tests) 
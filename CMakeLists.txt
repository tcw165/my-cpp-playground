cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(cpp-playground VERSION 0.1.0)

# You may want to use vcpkg for including the dependencies
# e.g. vcpkg install catch2 rxcpp boost
find_package(Threads REQUIRED)
find_package(Catch2 CONFIG REQUIRED)
find_package(rxcpp CONFIG REQUIRED)
# find_package(Boost REQUIRED)
# find_package(folly CONFIG REQUIRED)

file(GLOB SOURCES "*.cpp" "*.cc")

add_executable(fun ${SOURCES})
target_link_libraries(fun PRIVATE Threads::Threads)
target_link_libraries(fun PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_link_libraries(fun PRIVATE rxcpp)
# target_link_libraries(fun PRIVATE Boost::boost)
# target_link_libraries(fun PRIVATE Folly::folly Folly::folly_deps Folly::follybenchmark Folly::folly_test_util)

# Integrate Catch2 with CTest
# include(CTest)
# include(Catch)
# catch_discover_tests(fun)

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(cpp-playground VERSION 0.1.0)

include(${CMAKE_CURRENT_SOURCE_DIR}/papaya/CMakeLists.txt)

file(GLOB SOURCES "*.cpp" "*.cc")
add_executable(fun ${SOURCES})
target_include_directories(fun PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(fun PRIVATE papaya)

# You may want to use vcpkg for including the dependencies
# e.g. vcpkg install catch2 rxcpp boost

find_package(Threads REQUIRED)
target_link_libraries(fun PRIVATE Threads::Threads)

find_package(rxcpp CONFIG REQUIRED)
target_link_libraries(fun PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)

find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(fun PRIVATE rxcpp)

# find_package(Boost REQUIRED)
# target_link_libraries(fun PRIVATE Boost::boost)
# find_package(folly CONFIG REQUIRED)
# target_link_libraries(fun PRIVATE Folly::folly Folly::folly_deps Folly::follybenchmark Folly::folly_test_util)

# In order to use std::variant, ..., etc.
target_compile_features(fun PRIVATE cxx_std_17)

# Integrate Catch2 with CTest
include(CTest)
include(Catch)
catch_discover_tests(fun)

cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")

project(cpp-playground VERSION 0.1.0)

find_package(Catch2 CONFIG REQUIRED)
find_package(rxcpp CONFIG REQUIRED)

add_executable(tests tests-starter.cpp)
target_link_libraries(tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE rxcpp)

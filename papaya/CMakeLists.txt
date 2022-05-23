file(GLOB HEADERS "papaya/*.hpp" "papaya/factory/*.hpp")
file(GLOB SOURCES "papaya/*.cpp" "papaya/factory/*.cpp")
add_library(papaya ${SOURCES})
target_include_directories(papaya PRIVATE ${CMAKE_SOURCE_DIR})

# You may want to use vcpkg for including the dependencies
# e.g. vcpkg install catch2 rxcpp boost

find_package(rxcpp CONFIG REQUIRED)
target_link_libraries(papaya PRIVATE Threads::Threads)

find_package(Threads REQUIRED)
target_link_libraries(papaya PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)

find_package(Catch2 CONFIG REQUIRED)
target_link_libraries(papaya PRIVATE rxcpp)

# In order to use std::variant, ..., etc.
target_compile_features(papaya PRIVATE cxx_std_17)

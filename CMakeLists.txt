cmake_minimum_required(VERSION 3.8)
project(Boost-Test)
include_directories(.)
set(CMAKE_CXX_STANDARD 17)
add_executable(main ./main.cc)

enable_testing()
add_test(main-test main)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "lazy_matrix.hpp"
)
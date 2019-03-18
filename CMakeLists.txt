cmake_minimum_required(VERSION 3.8)
project(Boost-Test)
include_directories(.)
set(CMAKE_CXX_STANDARD 17)
find_package(OpenMP)

if (OPENMP_FOUND)
    message (STATUS "Using OpenMP for Parallel For loops")
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_executable(main ./main.cc)

enable_testing()
add_test(main-test main)

install(
    DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING PATTERN "matrix.hpp"
)
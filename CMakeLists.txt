project(CompyleTimeSQL)
cmake_minimum_required(VERSION 3.10)

include(conanbuildinfo.cmake)
conan_basic_setup()

SET(CMAKE_CXX_STANDARD 17)

SET(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${INCLUDE_DIR})

add_subdirectory(examples)
add_subdirectory(tests)
add_subdirectory(benchmark)

enable_testing()


project(CompyleTimeSQL)
cmake_minimum_required(VERSION 3.10)

include(conanbuildinfo.cmake)
conan_basic_setup()

SET(CMAKE_CXX_STANDARD 17)

SET(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
SET(SOURCE_DIR ${CMAKE_SOURCE_DIR}/source)

INCLUDE_DIRECTORIES(${INCLUDE_DIR})

#add project files
FILE(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_SOURCE_DIR} ${SOURCE_DIR}/*.cpp)
FILE(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_SOURCE_DIR} ${INCLUDE_DIR}/*.hpp)

#ADD_DEFINITIONS(-fsanitize=address)
#ADD_DEFINITIONS(-fno-omit-frame-pointer)
#ADD_DEFINITIONS(-O3)
#SET(CMAKE_BUILD_TYPE "Release")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} )#-lasan
conan_target_link_libraries(${PROJECT_NAME})

enable_testing()
add_subdirectory(tests)



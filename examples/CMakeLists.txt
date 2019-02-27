project(examples)
cmake_minimum_required(VERSION 3.10)

SET(INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
SET(SOURCE_DIR ${PROJECT_SOURCE_DIR}/source)

INCLUDE_DIRECTORIES(${INCLUDE_DIR})

#add project files
FILE(GLOB_RECURSE SOURCES RELATIVE ${PROJECT_SOURCE_DIR} ${SOURCE_DIR}/*.cpp)
FILE(GLOB_RECURSE HEADERS RELATIVE ${PROJECT_SOURCE_DIR} ${INCLUDE_DIR}/*.hpp)

ADD_DEFINITIONS(-fsanitize=address)
ADD_DEFINITIONS(-fno-omit-frame-pointer)
ADD_DEFINITIONS(-std=c++17)
ADD_DEFINITIONS(-O0)
ADD_DEFINITIONS(-Wall -Wextra -Werror)
SET(CMAKE_BUILD_TYPE "Debug")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} -lasan)
conan_target_link_libraries(${PROJECT_NAME})


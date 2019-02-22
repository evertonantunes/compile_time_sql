project(sql_benchamrk)
cmake_minimum_required(VERSION 2.6)

SET(CMAKE_CXX_STANDARD 17)

if (UNIX AND NOT APPLE)
    SET(Project_LIBRARIES
        -lpthread
        -lrt
    )
    ADD_DEFINITIONS(-Werror)
    ADD_DEFINITIONS(-Wdouble-promotion)
    ADD_DEFINITIONS(-Wmaybe-uninitialized)
    ADD_DEFINITIONS(-Werror=return-type)

    SET(CMAKE_BUILD_TYPE Release)
    ADD_DEFINITIONS(-O3)
endif()

#add project files
file(GLOB_RECURSE SOURCES source/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(include/)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${Project_LIBRARIES})
conan_target_link_libraries(${PROJECT_NAME})

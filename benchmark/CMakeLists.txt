project(sql_benchmark)
cmake_minimum_required(VERSION 2.6)

if (UNIX AND NOT APPLE)
    SET(Project_LIBRARIES
        -lpthread
        -lrt
    )
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

SET(OUTPUT_FILE_PATH ${PROJECT_SOURCE_DIR}/reports)
execute_process(COMMAND git rev-parse --verify HEAD
                OUTPUT_VARIABLE OUTPUT_FILE_NAME
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_STRIP_TRAILING_WHITESPACE)
SET(OUTPUT_FILE_PATH_NAME ${OUTPUT_FILE_PATH}/${OUTPUT_FILE_NAME})

add_custom_target(run_benchmark ALL)
add_custom_command(TARGET run_benchmark COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} --benchmark_format=json > ${OUTPUT_FILE_PATH_NAME})
add_custom_command(TARGET run_benchmark COMMAND git add ${OUTPUT_FILE_PATH_NAME})
add_custom_command(TARGET run_benchmark COMMAND git commit -m "add new benchmark report" ${OUTPUT_FILE_PATH_NAME})

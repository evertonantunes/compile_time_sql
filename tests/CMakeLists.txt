project(tests)
cmake_minimum_required(VERSION 2.6)

INCLUDE_DIRECTORIES(include/)

SET(CMAKE_CXX_STANDARD 17)

if (UNIX AND NOT APPLE)
    SET(Project_LIBRARIES
        -lpthread
        -lrt
    )
    ADD_DEFINITIONS(-Wdouble-promotion)
    ADD_DEFINITIONS(-Wmaybe-uninitialized)
    ADD_DEFINITIONS(-Werror=return-type)

    SET(CMAKE_BUILD_TYPE Debug)
    ADD_DEFINITIONS(-O0)

    ADD_DEFINITIONS(-fsanitize=address)
    ADD_DEFINITIONS(-fno-omit-frame-pointer)
    LIST(INSERT Project_LIBRARIES 0 -lasan)
endif()

#add project files
file(GLOB_RECURSE SOURCES source/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${Project_LIBRARIES})
conan_target_link_libraries(${PROJECT_NAME})

enable_testing()
add_test(MyTest ${PROJECT_NAME})

add_custom_target( run_test
                COMMAND  ${PROJECT_NAME} --reporter junit --out ${CMAKE_SOURCE_DIR}/report.xml
                COMMENT "${PROJECT_NAME} --reporter junit --out ${CMAKE_SOURCE_DIR}/report.xml"
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/tests
                VERBATIM )

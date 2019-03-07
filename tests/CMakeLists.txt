project(tests)
cmake_minimum_required(VERSION 2.6)

INCLUDE_DIRECTORIES(include/)

if (UNIX AND NOT APPLE)
    SET(Project_LIBRARIES )

    SET(CMAKE_BUILD_TYPE Debug)
    ADD_DEFINITIONS(-O0)

    ADD_DEFINITIONS(-fsanitize=address)
    ADD_DEFINITIONS(-fno-omit-frame-pointer)
    LIST(INSERT Project_LIBRARIES 0 -lasan)
endif()

ADD_DEFINITIONS(-DTEST__EXCEPTIONALIZE_STATIC_ASSERT=1)

#add project files
file(GLOB_RECURSE SOURCES source/*.cpp)
file(GLOB_RECURSE HEADERS include/*.hpp)

include_directories(${PROJECT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} ${Project_LIBRARIES})
conan_target_link_libraries(${PROJECT_NAME})

enable_testing()
add_test(NAME basic COMMAND  ${PROJECT_NAME} --reporter junit --out ${CMAKE_SOURCE_DIR}/report.xml )

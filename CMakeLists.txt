cmake_minimum_required(VERSION 3.25)

project(MiaAlgorithm LANGUAGES C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/raylib)
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/cglm)
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/googletest)
add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/benchmark)

set(SRC_DIR "${CMAKE_SOURCE_DIR}/src")
set(DEMO_DIR "${CMAKE_SOURCE_DIR}/demo")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/test")
set(INC_DIRS 
    "${SRC_DIR}"
    "${SRC_DIR}/aabbtree"
    "${SRC_DIR}/arena"
    "${SRC_DIR}/arrlist"
    "${SRC_DIR}/list"
    "${SRC_DIR}/pool"
    "${CMAKE_SOURCE_DIR}/vendor/raylib/src"
    "${CMAKE_SOURCE_DIR}/vendor/cglm/include"
    "${CMAKE_SOURCE_DIR}/vendor/googletest/googletest/include"
    "${CMAKE_SOURCE_DIR}/vendor/benchmark/include"
)

macro(create_lib target libs)
    add_library(${target} "${SRC_DIR}/${target}/${target}.c")
    target_include_directories(${target} PUBLIC ${INC_DIRS})
    target_link_libraries(${target} PUBLIC ${libs})
endmacro()

macro(create_demo target libs)
    set(target_demo "${target}_demo")
    add_executable(${target_demo} "${DEMO_DIR}/${target}/main.cpp")
    target_include_directories(${target_demo} PRIVATE ${INC_DIRS})
    target_link_libraries(${target_demo} PRIVATE ${target})
    target_link_libraries(${target_demo} PRIVATE ${libs})
    target_compile_options(${target_demo} PRIVATE -Wall -Wextra)
endmacro()
macro(create_demo_nolink target)
    set(target_demo "${target}_demo")
    add_executable(${target_demo} "${DEMO_DIR}/${target}/main.cpp")
    target_include_directories(${target_demo} PRIVATE ${INC_DIRS})
    target_compile_options(${target_demo} PRIVATE -Wall -Wextra)
endmacro()

macro(create_test target libs)
    set(target_test "${target}_test")
    add_executable(${target_test} "${TEST_DIR}/${target}/main.cpp")
    target_include_directories(${target_test} PRIVATE ${INC_DIRS})
    target_link_libraries(${target_test} PRIVATE ${target})
    target_link_libraries(${target_test} PRIVATE gtest)
    target_compile_options(${target_test} PRIVATE -Wall -Wextra)
endmacro()
macro(create_test_nolink target)
    set(target_test "${target}_test")
    add_executable(${target_test} "${TEST_DIR}/${target}/main.cpp")
    target_include_directories(${target_test} PRIVATE ${INC_DIRS})
    target_link_libraries(${target_test} PRIVATE gtest)
    target_compile_options(${target_test} PRIVATE -Wall -Wextra)
endmacro()

enable_testing()

create_lib(aabbtree "")

create_demo(aabbtree raylib)

create_test_nolink(arrlist)
create_test_nolink(list)

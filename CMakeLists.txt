cmake_minimum_required(VERSION 3.15)
project(structural_testing_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_COVERAGE "Enable GCC/Clang coverage instrumentation" OFF)

add_library(order_calc src/order_calc.cpp)
target_include_directories(order_calc PUBLIC include)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(order_calc PRIVATE --coverage -O0 -g)
    target_link_options(order_calc PRIVATE --coverage)
endif()

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(order_calc_tests tests/order_calc_tests.cpp)
target_link_libraries(order_calc_tests PRIVATE order_calc GTest::gtest_main)

if(ENABLE_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(order_calc_tests PRIVATE --coverage -O0 -g)
    target_link_options(order_calc_tests PRIVATE --coverage)
endif()

include(GoogleTest)
gtest_discover_tests(order_calc_tests)

cmake_minimum_required(VERSION 3.16)

project(pickup_point LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(CTest)

find_package(PostgreSQL REQUIRED)
find_path(LIBPQXX_INCLUDE pqxx/pqxx REQUIRED)
find_library(LIBPQXX_LIB NAMES pqxx libpqxx REQUIRED)

add_library(pickup_database src/db.cpp src/db.h)
target_include_directories(pickup_database PUBLIC src ${LIBPQXX_INCLUDE})
target_link_libraries(pickup_database PUBLIC ${LIBPQXX_LIB} PostgreSQL::PostgreSQL)

option(BUILD_GUI "Build the FLTK desktop application" ON)

if(BUILD_GUI)
    find_package(FLTK REQUIRED)

    add_executable(pickup_point
        src/main.cpp
        src/main_window.cpp
        src/main_window.h
        src/dialogs.cpp
        src/dialogs.h
        src/theme.h
    )
    target_include_directories(pickup_point PRIVATE src ${FLTK_INCLUDE_DIR})
    target_link_libraries(pickup_point PRIVATE pickup_database ${FLTK_LIBRARIES})
endif()

if(BUILD_TESTING)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(database_tests tests/database_tests.cpp)
    target_link_libraries(database_tests PRIVATE pickup_database GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(database_tests)
endif()

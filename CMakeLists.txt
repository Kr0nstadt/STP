cmake_minimum_required(VERSION 3.15)
project(playlist_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(playlist src/Playlist.cpp)
target_include_directories(playlist PUBLIC include)

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(playlist_tests tests/PlaylistTests.cpp)
target_link_libraries(playlist_tests PRIVATE playlist GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(playlist_tests)

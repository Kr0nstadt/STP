cmake_minimum_required(VERSION 3.15)

project(TaskManager_FLTK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)
include(CTest)

add_library(task_manager_domain
    src/task.cpp
    src/task.h
    src/project.cpp
    src/project.h
    src/repository.h
    src/taskmanager.cpp
    src/taskmanager.h
)
target_include_directories(task_manager_domain PUBLIC src)

option(BUILD_GUI "Build the FLTK desktop application" ON)

if(BUILD_GUI)
    FetchContent_Declare(
        fltk
        URL https://github.com/fltk/fltk/releases/download/release-1.4.5/fltk-1.4.5-source.tar.gz
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(FLTK_BUILD_TEST OFF CACHE BOOL "" FORCE)
    set(FLTK_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(FLTK_BUILD_FLUID OFF CACHE BOOL "" FORCE)
    set(FLTK_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(fltk)

    add_executable(TaskManager_FLTK
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
    )
    target_link_libraries(TaskManager_FLTK PRIVATE task_manager_domain fltk::fltk)

    if(WIN32)
        set_target_properties(TaskManager_FLTK PROPERTIES WIN32_EXECUTABLE TRUE)
    endif()
endif()

if(BUILD_TESTING)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    add_executable(task_manager_tests tests/task_manager_tests.cpp)
    target_link_libraries(task_manager_tests PRIVATE task_manager_domain GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(task_manager_tests)
endif()

cmake_minimum_required(VERSION 3.16)
project(parallel_grep LANGUAGES CXX)

find_package(Threads REQUIRED)
add_executable(pgrep src/main.cpp)
target_compile_features(pgrep PRIVATE cxx_std_17)
target_link_libraries(pgrep PRIVATE Threads::Threads)

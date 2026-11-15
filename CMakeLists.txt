cmake_minimum_required(VERSION 3.16)
project(Kubokop LANGUAGES CXX)

find_package(PostgreSQL REQUIRED)
add_executable(kubokop src/main.cpp src/cli.cpp src/game.cpp src/postgres_repository.cpp)
target_compile_features(kubokop PRIVATE cxx_std_17)
target_link_libraries(kubokop PRIVATE PostgreSQL::PostgreSQL)

if(MSVC)
    target_compile_options(kubokop PRIVATE /utf-8)
endif()


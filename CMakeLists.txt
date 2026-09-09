cmake_minimum_required(VERSION 3.20)

project(character_manager)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8)
endif()

add_executable(character_manager
    src/main.cpp
)


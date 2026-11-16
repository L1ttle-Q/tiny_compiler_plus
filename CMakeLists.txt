cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_COMPILER g++ CACHE STRING "C++ compiler")

project(tiny_compiler_plus LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(tiny
    src/compiler/main.cpp
    src/compiler/util.cpp
    src/compiler/scan.cpp
    src/compiler/parse.cpp
    src/compiler/dot.cpp
    src/compiler/symtab.cpp
    src/compiler/analyze.cpp
    src/compiler/code.cpp
    src/compiler/cgen.cpp
)
target_include_directories(tiny PRIVATE include/compiler)

add_executable(tm src/vm/tm.cpp)

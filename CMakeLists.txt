cmake_minimum_required(VERSION 3.16)
project(tiny_compiler_plus LANGUAGES CXX)
add_executable(tiny TC/main.cpp TC/util.cpp TC/scan.cpp TC/parse.cpp TC/dot.cpp
    TC/symtab.cpp TC/analyze.cpp TC/code.cpp TC/cgen.cpp)
add_executable(tm TM/tm.cpp)
target_compile_features(tiny PRIVATE cxx_std_11)
target_compile_features(tm PRIVATE cxx_std_11)

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 3.8 REQUIRED COMPONENTS Interpreter)
    foreach(case integer float condition undefined syntax triangle legacy_sample legacy_calc)
        add_test(NAME tiny_${case}
            COMMAND ${Python3_EXECUTABLE} ${PROJECT_SOURCE_DIR}/tests/run_tests.py
                --compiler $<TARGET_FILE:tiny> --machine $<TARGET_FILE:tm> --case ${case})
        set_tests_properties(tiny_${case} PROPERTIES TIMEOUT 40)
    endforeach()
endif()

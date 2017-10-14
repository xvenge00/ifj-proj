cmake_minimum_required(VERSION 3.6)
project(ifj_proj)

include(tests/CMakeLists.txt)
enable_testing()


set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES  source/codegen.c source/main.c source/symtable.c source/str_buff.c source/memwork.c  source/parser.c source/scanner.c)

add_executable(ifj_proj ${SOURCE_FILES})

add_test(NAME test COMMAND tmp)

set(SOURCE_FILES_STR_BUFF tests/test_str_buff.cpp
        source/str_buff.c source/str_buff.h source/memwork.c source/memwork.h
        source/main.h)
add_executable(str_buff ${SOURCE_FILES_STR_BUFF})
target_link_libraries(str_buff gtest)

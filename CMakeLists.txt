cmake_minimum_required(VERSION 3.6)
project(ifj_proj)
set(CMAKE_CXX_STANDARD 11)

include(tests/CMakeLists.txt)
enable_testing()


set(SOURCE_FILES  source/codegen.c source/main.c source/symtable.c
        source/str_buff.c source/memwork.c  source/parser.c source/scanner.c source/expression.c source/expression.h source/token_stack.c source/token_stack.h source/err.c source/err.h)
add_executable(ifj_proj ${SOURCE_FILES})

#testy memwork
set(SOURCE_FILES_MEMWORK source/memwork.h source/memwork.c)
add_executable(memwork ${SOURCE_FILES_MEMWORK} tests/memwork_test.cpp)
target_link_libraries(memwork gtest)

#testy str_buff
set(SOURCE_FILES_STR_BUFF
        source/str_buff.c source/str_buff.h ${SOURCE_FILES_MEMWORK}
        source/err.h)
add_executable(str_buff ${SOURCE_FILES_STR_BUFF} tests/test_str_buff.cpp)
target_link_libraries(str_buff gtest)

#testy symtable
set(SOURCE_FILES_SYMTABLE source/symtable.h source/symtable.c ${SOURCE_FILES_STR_BUFF})
add_executable(symtable ${SOURCE_FILES_SYMTABLE} tests/symtable_test.cpp)
target_link_libraries(symtable gtest)

#testy scanner
set(SOURCE_FILES_SCANNER source/scanner.h source/scanner.c ${SOURCE_FILES_STR_BUFF} ${SOURCE_FILES_MEMWORK})
add_executable(scanner ${SOURCE_FILES_SCANNER} tests/scanner_test.cpp)
target_link_libraries(scanner gtest)

add_executable(debug ${SOURCE_FILES_SCANNER} tests/debug_scanner.c)
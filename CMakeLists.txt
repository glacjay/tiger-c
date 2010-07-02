cmake_minimum_required(VERSION 2.6)
project(tiger-c)

include(FindFLEX)

flex_target(LEXER lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
add_executable(tiger
    ast.c errmsg.c main.c utils.c
    ${FLEX_LEXER_OUTPUTS})

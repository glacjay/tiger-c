cmake_minimum_required(VERSION 2.6)
project(tiger-c)

include(FindFLEX)
include(FindBISON)

flex_target(LEXER lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
bison_target(PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
add_flex_bison_dependency(LEXER PARSER)

add_executable(tiger
    ast.c escape.c env.c errmsg.c frame-mips.c main.c ppast.c semantic.c
    symbol.c table.c temp.c translate.c types.c utils.c
    ${FLEX_LEXER_OUTPUTS}
    ${BISON_PARSER_OUTPUTS})

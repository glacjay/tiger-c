cmake_minimum_required(VERSION 3.0)
project(tiger-c)

include(FindFLEX)
include(FindBISON)

flex_target(LEXER lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.c)
bison_target(PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.c)
add_flex_bison_dependency(LEXER PARSER)

set_source_files_properties(lexer.l parser.y PROPERTIES HEADER_FILE_ONLY TRUE)

add_executable(tiger
    ast.c
    ast.h
    env.c
    env.h
    errmsg.c
    errmsg.h
    escape.c
    escape.h
    frame.h
    frame-mips.c
    ir.c
    ir.h
    lexer.l
    main.c
    parser-wrap.h
    parser.y
    ppast.c
    ppast.h
    ppir.c
    ppir.h
    semantic.c
    semantic.h
    symbol.c
    symbol.h
    table.c
    table.h
    temp.c
    temp.h
    translate.c
    translate.h
    types.c
    types.h
    utils.c
    utils.h
    ${FLEX_LEXER_OUTPUTS}
    ${BISON_PARSER_OUTPUTS}
)

CMAKE_MINIMUM_REQUIRED(VERSION 3.20)
PROJECT(ccom)

include_directories(${CMAKE_SOURCE_DIR}/include)


add_executable(ccom main.c
        include/ccom/frontend/lexer/tokenize/tokenize.h
        source/ccom/frontend/lexer/tokenize/tokenize.c
        include/ccom/utils/error.h
        include/ccom/utils/error.h
        source/ccom/utils/error.c
        include/ccom/common/globals.h
        source/ccom/common/globals.c
        include/ccom/backend/codegen/asmcode.h
        source/ccom/backend/codegen/asmcode.c
        include/ccom/frontend/ast/ast.h
        include/ccom/utils/string.h
        include/ccom/frontend/parser/parser.h
        include/ccom/frontend/parser/token.h
        source/ccom/frontend/parser/token.c
        source/ccom/frontend/parser/parser.c)
//
// Created by aowei on 2025/8/17.
//

#include <stdio.h>
#include <ccom/utils/error.h>
#include <ccom/common/globals.h>

#include "ccom/backend/codegen/asmcode.h"
#include "ccom/frontend/ast/ast.h"
#include "ccom/frontend/parser/parser.h"

int main(const int argc, char **argv) {
    if (argc != 2) {
        error("%s: invalid number of arguments!", argv[0]);
    }
    g_user_input_base = argv[1];
    g_current_token = tokenize();
    const ASTNode *node = expression();
    codegen(node);
}

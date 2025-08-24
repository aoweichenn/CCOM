//
// Created by aowei on 2025/8/24.
//

#include <string.h>
#include <ccom/utils/error.h>
#include <ccom/common/globals.h>
#include <ccom/frontend/parser/token.h>

bool consume(const char *op) {
    if (g_current_token->type != TOKEN_RESERVED || strlen(op) != g_current_token->length ||
        strncmp(g_current_token->string, op, g_current_token->length)) {
        return false;
    }
    g_current_token = g_current_token->next_token;
    return true;
}

void except(const char *op) {
    if (g_current_token->type != TOKEN_RESERVED || strlen(op) != g_current_token->length ||
        strncmp(g_current_token->string, op, g_current_token->length)) {
        error_at(g_current_token->string, "excepted \"%s\"", op);
    }
    g_current_token = g_current_token->next_token;
}

long except_number(void) {
    if (g_current_token->type != TOKEN_NUMBER) {
        error_at(g_current_token->string, "excepted a number.");
    }
    const long value = g_current_token->value;
    g_current_token = g_current_token->next_token;
    return value;
}

bool at_eof(void) {
    return g_current_token->type == TOKEN_EOF;
}

//
// Created by aowei on 2025/8/17.
//

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
// #include <ccom/tokenize/tokenize.h>


typedef enum {
    TOKEN_RESERVED,
    TOKEN_NUMBER,
    TOKEN_EOF,
} TokenType;


typedef struct Token {
    TokenType type;
    struct Token *next;
    int value;
    char *string;
} Token;


Token *token;

void error(char *fmt, ...) {
    va_list ap = {};
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(const char op) {
    if (token->type != TOKEN_RESERVED || token->string[0] != op) {
        return false;
    }
    token = token->next;
    return true;
}

void expect(const char op) {
    if (token->type != TOKEN_RESERVED || token->string[0] != op) {
        error("expect: '%c'", op);
    }
    token = token->next;
}

int expect_number() {
    if (token->type != TOKEN_NUMBER) {
        error("not number!");
    }
    const int val = token->value;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->type == TOKEN_EOF;
}

Token *new_token(TokenType type, Token *cur, char *string) {
    Token *ptoken = calloc(1, sizeof(Token));
    ptoken->type = type;
    ptoken->string = string;
    cur->next = ptoken;
    return ptoken;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while (*p) {
        if (isspace((*p))) {
            p++;
            continue;
        }
        if (*p == '+' || *p == '-') {
            cur = new_token(TOKEN_RESERVED, cur, p++);
            continue;
        }
        if (isdigit(*p)) {
            cur = new_token(TOKEN_NUMBER, cur, p);
            cur->value = strtol(p, &p, 10);
            continue;
        }
        error("unknow char: %c", *p);
    }
    new_token(TOKEN_EOF, cur, p);
    return head.next;
}


int main(const int argc, char **argv) {
    if (argc != 2) {
        error("参数数量不正确");
        return 1;
    }
    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("    mov rax, %d\n", expect_number());

    while (!at_eof()) {
        if (consume('+')) {
            printf("    add rax,%d\n", expect_number());
            continue;
        }
        expect('-');
        printf("    sub rax, %d\n", expect_number());
    }
    printf("    ret\n");
    return 0;
}

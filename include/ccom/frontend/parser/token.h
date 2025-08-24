//
// Created by aowei on 2025/8/24.
//

#ifndef CCOM_FRONTEND_PARSER_TOKEN_H
#define CCOM_FRONTEND_PARSER_TOKEN_H

#include <stdbool.h>

bool consume(const char *op);

void except(const char *op);

long except_number(void);

bool at_eof(void);

#endif //CCOM_FRONTEND_PARSER_TOKEN_H

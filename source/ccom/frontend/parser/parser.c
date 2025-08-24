//
// Created by aowei on 2025/8/24.
//

#include <stdlib.h>
#include <ccom/frontend/parser/token.h>
#include <ccom/frontend/parser/parser.h>

// 静态函数声明

static ASTNode *equality(void);

static ASTNode *relational(void);

static ASTNode *add(void);

static ASTNode *mul(void);

static ASTNode *unary(void);

static ASTNode *primary(void);


// 静态函数定义
static ASTNode *new_node(const ASTNodeType type) {
    ASTNode *node = calloc(1, sizeof(ASTNode));
    node->type = type;
    return node;
}

static ASTNode *new_binary_node(const ASTNodeType type, ASTNode *lhs, ASTNode *rhs) {
    ASTNode *node = new_node(type);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

static ASTNode *new_number_node(const long value) {
    ASTNode *node = new_node(AST_NODE_NUMBER);
    node->value = value;
    return node;
}

ASTNode *expression(void) {
    return equality();
}

static ASTNode *equality(void) {
    ASTNode *node = relational();
    for (;;) {
        if (consume("==")) {
            node = new_binary_node(AST_NODE_EQ, node, relational());
        } else if (consume("!=")) {
            node = new_binary_node(AST_NODE_NE, node, relational());
        } else {
            return node;
        }
    }
}

static ASTNode *relational(void) {
    ASTNode *node = add();
    for (;;) {
        if (consume("<")) {
            node = new_binary_node(AST_NODE_LT, node, add());
        } else if (consume("<=")) {
            node = new_binary_node(AST_NODE_LE, node, add());
        } else if (consume(">")) {
            node = new_binary_node(AST_NODE_LT, add(), node);
        } else if (consume(">=")) {
            node = new_binary_node(AST_NODE_LT, add(), node);
        } else {
            return node;
        }
    }
}

static ASTNode *add(void) {
    ASTNode *node = mul();
    for (;;) {
        if (consume("+")) {
            node = new_binary_node(AST_NODE_ADD, node, mul());
        } else if (consume("-")) {
            node = new_binary_node(AST_NODE_SUB, node, mul());
        } else {
            return node;
        }
    }
}

static ASTNode *mul(void) {
    ASTNode *node = unary();
    for (;;) {
        if (consume("*")) {
            node = new_binary_node(AST_NODE_MUL, node, unary());
        } else if (consume("/")) {
            node = new_binary_node(AST_NODE_DIV, node, unary());
        } else {
            return node;
        }
    }
}

static ASTNode *unary(void) {
    if (consume("+")) {
        return unary();
    }
    if (consume("-")) {
        return new_binary_node(AST_NODE_SUB, new_number_node(0), unary());
    }
    return primary();
}

static ASTNode *primary(void) {
    if (consume("(")) {
        ASTNode *node = expression();
        except(")");
        return node;
    }
    return new_number_node(except_number());
}

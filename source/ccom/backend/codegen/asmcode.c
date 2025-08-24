//
// Created by aowei on 2025/8/24.
//

#include <stdio.h>
#include <ccom/utils/error.h>
#include <ccom/backend/codegen/asmcode.h>


static void gen_asm_code(const ASTNode *node) {
    if (node->type == AST_NODE_NUMBER) {
        printf("    push %ld\n", node->value);
        return;
    }
    gen_asm_code(node->lhs);
    gen_asm_code(node->rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->type) {
        case AST_NODE_ADD:
            printf("    add rax, rdi\n");
            break;
        case AST_NODE_SUB:
            printf("    sub rax, rdi\n");
            break;
        case AST_NODE_MUL:
            printf("    imul rax, rdi\n");
            break;
        case AST_NODE_DIV:
            printf("     cqo\n");
            printf("     idiv rdi\n");
            break;
        case AST_NODE_EQ:
            printf("     cmp rax, rdi\n");
            printf("     sete al\n");
            printf("     movzb rax, al\n");
            break;
        case AST_NODE_NE:
            printf("     cmp rax, rdi\n");
            printf("     setne al\n");
            printf("     movzb rax, al\n");
            break;
        case AST_NODE_LT:
            printf("     cmp rax, rdi\n");
            printf("     setl al\n");
            printf("     movzb rax, al\n");
            break;
        case AST_NODE_LE:
            printf("     cmp rax, rdi\n");
            printf("     setle al\n");
            printf("     movzb rax, al\n");
            break;
        default: error("This AST Node Type: %s is not supported now!", node->type);
    }
    printf("    push rax\n");
}

void codegen(const ASTNode *node) {
    const char *start = ".intel_syntax noprefix\n.global main\nmain:\n";
    const char *end = "    pop rax\n    ret\n";
    printf("%s", start);
    gen_asm_code(node);
    printf("%s", end);
}

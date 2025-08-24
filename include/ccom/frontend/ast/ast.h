//
// Created by aowei on 2025/8/24.
//

#ifndef CCOM_FRONTEND_AST_AST_H
#define CCOM_FRONTEND_AST_AST_H

/**
 * @brief AST（抽象语法树）节点类型枚举
 *
 * 用于区分不同类型的AST节点，主要涵盖表达式相关的节点（算术运算、比较运算、常量等），
 * 在语法解析、语义分析、代码生成等阶段通过节点类型判断具体逻辑。
 */
typedef enum {
    AST_NODE_ADD, // 加法表达式节点（对应运算符 '+'，如 a + b）
    AST_NODE_SUB, // 减法表达式节点（对应运算符 '-'，如 a - b）
    AST_NODE_MUL, // 乘法表达式节点（对应运算符 '*'，如 a * b）
    AST_NODE_DIV, // 除法表达式节点（对应运算符 '/'，如 a / b）
    AST_NODE_EQ, // 等于比较节点（对应运算符 '=='，如 a == b）
    AST_NODE_NE, // 不等于比较节点（对应运算符 '!='，如 a != b）
    AST_NODE_LT, // 小于比较节点（对应运算符 '<'，如 a < b）
    AST_NODE_LE, // 小于等于比较节点（对应运算符 '<='，如 a <= b）
    AST_NODE_NUMBER // 数字常量节点（表示整数/浮点数等字面量，目前支支持整数，如 42、3.14）
} ASTNodeType;

/**
 * @brief 抽象语法树（AST）的通用节点结构体
 *
 * 该结构体是AST的基础构建块，用于表示源代码中的各种语法元素（如表达式、常量等）。
 * 通过不同的`type`字段区分节点类型，并根据类型使用对应的成员变量，实现对不同语法结构的统一抽象。
 * 适用于语法解析、语义分析及代码生成等阶段对语法结构的表示和处理。
 */
typedef struct ASTNode {
    /**
     * 节点类型标识，用于区分当前节点是哪种语法元素（如加法、数字常量等）。
     * 取值来自ASTNodeType枚举（如AST_NODE_ADD表示加法节点，AST_NODE_NUMBER表示数字常量节点）。
     * 是节点的"标签"，决定了如何解析和处理该节点的其他成员。
     */
    ASTNodeType type;

    /**
     * 左操作数指针，指向当前节点的左侧子节点（仅适用于二元运算符节点）。
     * 例如：对于加法节点（AST_NODE_ADD），lhs指向表示"左操作数表达式"的ASTNode（如a + b中的a）。
     * 对于非二元运算符节点（如数字常量AST_NODE_NUMBER），该字段为NULL。
     */
    struct ASTNode *lhs;

    /**
     * 右操作数指针，指向当前节点的右侧子节点（仅适用于二元运算符节点）。
     * 例如：对于加法节点（AST_NODE_ADD），rhs指向表示"右操作数表达式"的ASTNode（如a + b中的b）。
     * 对于非二元运算符节点（如数字常量AST_NODE_NUMBER），该字段为NULL。
     */
    struct ASTNode *rhs;

    /**
     * 数字常量值，仅在节点类型为数字常量（AST_NODE_NUMBER）时有效。
     * 用于存储源代码中的整数字面量（如123、-45等），其他类型节点（如运算符节点）中该字段无意义。
     */
    long value;
} ASTNode;

#endif //CCOM_FRONTEND_AST_AST_H

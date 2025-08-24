//
// Created by aowei on 2025/8/20.
//

#ifndef CCOM_TOKENIZE_H
#define CCOM_TOKENIZE_H

#include <stdbool.h>
/**
 * @brief 词法分析器（Lexer）生成的Token类型枚举
 *
 * 用于标识词法分析过程中识别出的不同类型的Token（词法单元），
 * 是语法分析器（Parser）区分输入元素的基础依据，帮助解析器判断如何处理当前Token。
 */
typedef enum {
    TOKEN_RESERVED, // 保留字或运算符等特殊符号Token，如关键字（if、else）、运算符（+、-、*、/）、标点符号（;、,、(){}）等
    TOKEN_NUMBER, // 数字常量Token，表示源代码中的整数字面量（如123、456）或浮点数字面量（根据实现可能扩展）
    TOKEN_EOF // 输入结束Token（End Of File），标识词法分析器已读取完所有输入内容，无更多Token
} TokenType;

/**
 * @brief 词法单元（Token）结构体
 *
 * 表示词法分析器（Lexer）从源代码中解析出的最小词法单元，是连接词法分析与语法分析的核心数据结构。
 * 每个Token包含类型、值、字符串表示及链表指针，通过链表形式构成"Token流"，供语法分析器（Parser）按顺序读取和处理。
 */
typedef struct Token {
    /**
     * @brief Token的类型标识
     *
     * 用于区分当前Token的类别，取值来自TokenType枚举（如TOKEN_RESERVED表示保留字/运算符，
     * TOKEN_NUMBER表示数字常量，TOKEN_EOF表示输入结束）。是语法分析器判断如何处理该Token的核心依据。
     */
    TokenType type;

    /**
     * @brief 指向链表中下一个Token的指针
     *
     * 词法分析器生成的Token通过该指针串联成单向链表（Token流），语法分析器通过遍历该链表依次获取Token，
     * 实现对源代码的顺序解析。若当前Token是最后一个，则该指针为NULL。
     */
    struct Token *next_token;

    /**
     * @brief 数字常量的数值
     *
     * 仅当Token类型为TOKEN_NUMBER时有效，用于存储源代码中数字字面量的数值（如"123"对应的value为123，
     * "-45"对应的value为-45）。对于其他类型的Token（如保留字、运算符），该字段无实际意义。
     */
    long value;

    /**
     * @brief Token对应的原始字符串
     *
     * 存储该Token在源代码中的字符序列（如保留字"if"对应的string为"if"，运算符"+"对应的string为"+"，
     * 标识符"count"对应的string为"count"）。用于错误提示（如"Unexpected token 'xxx'"）或调试。
     */
    char *string;

    /**
     * @brief string成员的字符长度（不包含字符串终止符'\0'）
     *
     * 预存string的长度，避免频繁调用strlen()计算，提高字符串操作效率（如比较Token字符串、截取子串时）。
     * 例如，string为"if"时，length为2。
     */
    int length;
} Token;

Token *tokenize(void);


#endif //CCOM_TOKENIZE_H

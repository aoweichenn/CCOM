//
// Created by aowei on 2025/8/20.
//

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include <ccom/common/globals.h>
#include <ccom/frontend/lexer/tokenize/tokenize.h>

#include "ccom/utils/error.h"

/**
 * @brief 检查主字符串是否以指定前缀字符串开头
 *
 * 该函数通过比较主字符串和前缀字符串的前n个字符（n为前缀字符串的长度），
 * 判断主字符串是否以前缀字符串起始。适用于词法分析中识别关键字前缀、语法规则匹配等场景。
 *
 * @param[in] string 被检查的主字符串（如源代码中的字符序列）
 *                   类型为const char*，表示函数不会修改该字符串内容，增强代码安全性
 * @param[in] prefix 用于匹配的前缀字符串（如关键字"if"、运算符">="的前缀">"）
 *                   类型为const char*，同样保证字符串内容不被修改
 *
 * @return bool 匹配结果：
 *             - true：主字符串的前strlen(prefix)个字符与前缀字符串完全一致（即主字符串以前缀开头）
 *             - false：主字符串不以该前缀开头
 *
 * @note 关键实现逻辑：
 *       1. 调用strlen(prefix)获取前缀字符串的长度n（不包含终止符'\0'）；
 *       2. 使用strncmp函数比较主字符串和前缀字符串的前n个字符；
 *       3. 若strncmp返回0（表示前n个字符完全匹配），则返回true，否则返回false。
 *
 * @note 边界情况：
 *       - 若prefix为空字符串（strlen(prefix) == 0），strncmp始终返回0，函数返回true（空字符串是任何字符串的前缀）；
 *       - 若string长度小于prefix长度，strncmp会比较到string的终止符，此时必然不匹配，返回false。
 *
 * @attention 函数被static修饰，说明其作用域仅限于当前编译单元（.c文件），
 *            是模块内部的辅助工具函数，不对外暴露，避免命名冲突和外部依赖。
 */
static bool startswith(const char *string, const char *prefix) {
    // 比较主字符串和前缀字符串的前strlen(prefix)个字符，若相等则返回true
    return strncmp(string, prefix, strlen(prefix)) == 0;
}

/**
 * @brief 创建并初始化一个新的Token节点，并将其添加到Token链表的尾部
 *
 * 该函数是词法分析器（tokenize）的内部辅助函数，负责为解析出的词法单元（Token）
 * 分配内存、初始化成员，并通过链表指针将新节点链接到现有Token流的尾部，确保Token流的连续性。
 * 函数被static修饰，仅在当前编译单元（.c文件）内可见，避免与其他模块的函数命名冲突。
 *
 * @param[in] type 新Token的类型（来自TokenType枚举，如TOKEN_RESERVED表示运算符，TOKEN_NUMBER表示数字）
 *                 用const修饰，表明该参数在函数内不会被修改，增强代码安全性
 * @param[in,out] current 指向当前Token链表尾部节点的指针（即新节点要链接到的前一个节点）
 *                        函数会修改该节点的next_token指针，使其指向新创建的节点
 * @param[in] string 新Token对应的原始字符串（如运算符"+"、数字"123"的字符序列）
 *                   通常指向源代码字符串中的对应位置，避免额外内存拷贝
 * @param[in] length 新Token字符串的长度（不包含字符串终止符'\0'）
 *                   用const修饰，表明长度在函数内不会被修改，且提前计算好长度可避免频繁调用strlen
 *
 * @return Token* 指向新创建的Token节点的指针
 *                调用者可通过该返回值更新链表尾部指针（current = new_token），方便后续添加更多Token
 *
 * @note 内存管理：使用calloc分配内存（会自动将所有字节初始化为0），确保新节点的未显式设置的成员（如next_token）
 *       默认为NULL，避免野指针风险。后续需通过专门的函数（如token_destroy）释放整个Token链表的内存，防止泄漏。
 *
 * @note 链表链接逻辑：通过修改current->next_token = new_token，将新节点挂到当前尾部节点之后，
 *       使新节点成为链表的新尾部，保证Token流按解析顺序串联。
 */
static Token *new_token(const TokenType type, Token *current, char *string, const int length) {
    // 为新Token节点分配内存（大小为Token结构体大小），并初始化为0（避免未初始化的成员导致的野指针）
    Token *new_token = calloc(1, sizeof(Token));

    // 设置新Token的类型（如运算符、数字、EOF等）
    new_token->type = type;

    // 保存Token对应的原始字符串（指向源代码中的对应位置，不拷贝内容，提高效率）
    new_token->string = string;

    // 设置字符串长度（提前计算好，避免后续重复调用strlen）
    new_token->length = length;

    // 将新节点链接到当前链表的尾部：当前尾部节点的next_token指向新节点
    current->next_token = new_token;

    // 返回新节点的指针，供调用者更新链表尾部指针（current = new_token）
    return new_token;
}

/**
 * @brief 词法分析器入口函数：将用户输入的源代码字符串转换为Token流（Token链表）
 *
 * 功能：逐个扫描源代码字符，根据字符类型（空白、双字符运算符、单字符运算符、数字等）
 *       生成对应的Token节点，通过链表串联形成Token流，供语法分析器（parser）后续处理。
 *       无法识别的字符会触发错误提示。
 *
 * @return Token* 生成的Token流的第一个有效节点（跳过哨兵节点）
 */
Token *tokenize(void) {
    // 获取用户输入源代码的起始地址，current_pos用于跟踪当前解析的字符位置
    char *current_pos = g_user_input_base;

    // 哨兵节点（dummy node）：用于简化链表操作（无需处理空链表的特殊情况）
    // 哨兵节点本身不存储有效数据，有效Token从head.next_token开始
    Token head = {};
    // current指针：始终指向当前Token链表的尾部，用于添加新Token
    Token *current = &head;

    // 循环解析源代码，直到遇到字符串结束符（'\0'）
    while (*current_pos) {
        // 1. 跳过空白字符（空格、制表符、换行符等，语法中无实际意义）
        if (isspace(*current_pos)) {
            current_pos++; // 指针后移，跳过当前空白字符
            continue; // 继续解析下一个字符
        }

        // 2. 处理双字符保留运算符（如==、!=、<=、>=）
        // 优先检查双字符运算符，避免被拆分为两个单字符（如将"=="误判为"="和"="）
        if (startswith(current_pos, "==") || startswith(current_pos, "!=") ||
            startswith(current_pos, "<=") || startswith(current_pos, ">=")) {
            // 创建保留符号Token（类型为TOKEN_RESERVED），长度为2
            // 新Token会被添加到链表尾部，current更新为新节点
            current = new_token(TOKEN_RESERVED, current, current_pos, 2);
            current_pos += 2; // 指针后移2位，跳过整个双字符运算符
            continue;
        }

        // 3. 处理单字符保留符号（运算符或标点，如+、-、;、(、)等）
        // ispunct()判断是否为标点符号（包含大部分语法所需的单字符运算符）
        if (ispunct(*current_pos)) {
            // 创建保留符号Token（类型为TOKEN_RESERVED），长度为1
            // current_pos++：先使用当前字符，再将指针后移1位
            current = new_token(TOKEN_RESERVED, current, current_pos++, 1);
            continue;
        }

        // 4. 处理数字常量（如123、45等整数）
        if (isdigit(*current_pos)) {
            // 创建数字类型Token（类型为TOKEN_NUMBER），初始长度暂为0
            current = new_token(TOKEN_NUMBER, current, current_pos, 0);
            // 记录数字的起始位置（用于后续计算数字字符串长度）
            char *number_start_pos = current_pos;
            // 解析整数：strtol将字符串转为长整数，第二个参数输出解析结束后的指针
            // 第三个参数10表示十进制
            current->value = strtol(current_pos, &current_pos, 10);
            // 计算数字字符串的长度（结束位置 - 起始位置）
            current->length = current_pos - number_start_pos;
            continue;
        }

        // 5. 无法识别的字符：触发错误提示（当前字符不符合任何已知Token规则）
        error_at(current_pos, "invalid token");
    }

    // 解析结束：添加EOF Token（标识输入结束），类型为TOKEN_EOF
    new_token(TOKEN_EOF, current, current_pos, 0);

    // 返回Token流的第一个有效节点（跳过哨兵节点head）
    return head.next_token;
}

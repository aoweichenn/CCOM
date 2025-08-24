//
// Created by aowei on 2025/8/24.
//

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <ccom/common/globals.h>
#include <ccom/utils/error.h>


/**
 * @brief 输出错误信息并终止程序
 *
 * 接收格式化字符串和可变参数，将错误信息输出到标准错误流 (stderr)
 * 追加换行后以失败状态码终止程序
 *
 * @param fmt_str char* 格式化字符串 (支持 printf 风格的格式说明符)，不可为 NULL
 * @param ... 和 fmt_str 匹配的可变参数列表
 * @note 函数调用后会直接终止程序，不会返回
 * @warning 如果 fmt_str 为 NULL，则会先输出内部错误信息再退出
 */
void error(const char *fmt_str, ...) {
    // 【防御性检查】避免空指针导致的未定义行为
    if (fmt_str == NULL) {
        fprintf(stderr, "Internal Error: error() called with NULL format string.\n");
        // 强制退出，避免后续操作崩溃
        exit(EXIT_FAILURE);
    }

    // 声明可变参数遍历器（C11中为不透明结构体，由stdarg.h定义）
    va_list args_ptr = {};
    // 初始化：让 args_ptr 指向 fmt_str 后的第一个可变参数
    va_start(args_ptr, fmt_str);

    // 格式化输出到标准错误流（vfprintf 是 printf 族中处理可变参数的函数）
    vfprintf(stderr, fmt_str, args_ptr);
    // 追加换行，保证错误信息独立一行
    fprintf(stderr, "\n");
    // 清理可变参数资源（必要步骤，即使后续 exit，也需遵循规范）
    va_end(args_ptr);

    // 终止程序，返回"失败"状态码（EXIT_FAILURE 由 stdlib.h 定义，通常为 1，比 -1 更规范、可移植）
    exit(EXIT_FAILURE);
}


void error_at(const char *location, const char *fmt_str, ...) {
    // 【防御性检查】避免空指针导致的未定义行为
    if (fmt_str == NULL) {
        fprintf(stderr, "Internal Error: error() called with NULL format string.\n");
        // 强制退出，避免后续操作崩溃
        exit(EXIT_FAILURE);
    }

    va_list args_ptr = {};
    va_start(args_ptr, fmt_str);

    const int position = location - g_user_input_base;
    fprintf(stderr, "%s\n", g_user_input_base);
    fprintf(stderr, "%*s\n", position, "");
    fprintf(stderr, "^ ");
    // 格式化输出到标准错误流（vfprintf 是 printf 族中处理可变参数的函数）
    vfprintf(stderr, fmt_str, args_ptr);
    // 追加换行，保证错误信息独立一行
    fprintf(stderr, "\n");
    // 清理可变参数资源（必要步骤，即使后续 exit，也需遵循规范）
    va_end(args_ptr);

    exit(EXIT_FAILURE);
}

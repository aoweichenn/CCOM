//
// Created by aowei on 2025/8/24.
//

#ifndef CCOM_UTILS_ERROR_H
#define CCOM_UTILS_ERROR_H

void error(const char *fmt_str, ...);

void error_at(const char *location, const char *fmt_str, ...);

#endif //CCOM_UTILS_ERROR_H

/**
 * @file nx_log.h
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
 *
 * This file is part of the VIRG-Nexus Library
 *
 * VIRG-Nexus Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * VIRG-Nexus Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_LOG_H
#define VIRG_NEXUS_NX_LOG_H

#include <stdarg.h>

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

typedef __NX_NO_RETURN_PTR void(*NXLogFatalFuncP)(const char* function_name, int line_no, const char* tag, const char* msg, va_list prm);

enum NXLogVerbosity {
        NX_LOG_INFORMATIVE = 0,
        NX_LOG_DEFAULT = 1,
        NX_LOG_CAUTIOUS = 2,
        NX_LOG_SILENT = 3
};

enum NXLogLevel {
        NX_LOG_INFO = 0,
        NX_LOG_NORMAL = 1,
        NX_LOG_WARNING = 2,
        NX_LOG_ERROR = 3
};

#define NX_INFO(tag, msg, ...)                                          \
        do {                                                            \
                nx_log_formatted(NX_LOG_INFO, __NX_FUNCTION, __LINE__, tag, msg, ##__VA_ARGS__); \
        } while(0)

#define NX_LOG(tag, msg, ...)                                           \
        do {                                                            \
                nx_log_formatted(NX_LOG_NORMAL, __NX_FUNCTION, __LINE__, tag, msg, ##__VA_ARGS__); \
        } while(0)

#define NX_WARNING(tag, msg, ...)                                       \
        do {                                                            \
                nx_log_formatted(NX_LOG_WARNING, __NX_FUNCTION, __LINE__, tag, msg, ##__VA_ARGS__); \
        } while(0)

#define NX_ERROR(tag, msg, ...)                                         \
        do {                                                            \
                nx_log_formatted(NX_LOG_ERROR, __NX_FUNCTION, __LINE__, tag, msg, ##__VA_ARGS__); \
        } while(0)

#define NX_FATAL(tag, msg, ...)                                         \
        do {                                                            \
                nx_log_fatal(__NX_FUNCTION, __LINE__, tag, msg, ##__VA_ARGS__); \
        } while(0)

void nx_log_formatted(enum NXLogLevel log_level, const char* function_name, int line_no, const char *tag, const char* msg, ...) __attribute__((format (printf, 5, 6)));
__NX_NO_RETURN void nx_log_fatal(const char* function_name, int line_no, const char *tag, const char* msg, ...) __attribute__((format (printf, 4, 5)));

void nx_log_verbosity (enum NXLogVerbosity verb); /**< Sets which logs get printed and which are suppressed. */
void nx_log_fatal_func(__NX_NO_RETURN_PTR NXLogFatalFuncP fatal_func);      /**< Change the function that gets called in case of a fatal error. */

__NX_END_DECL

#endif

/**
 * @file nx_message.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#ifndef VIRG_NEXUS_NX_MESSAGE_H
#define VIRG_NEXUS_NX_MESSAGE_H

#include <stdarg.h>

#include "virg/nexus/nx_config.h"

__NX_BEGIN_DECL

typedef __NX_NO_RETURN_PTR void(*NXFatalFuncP)(const char* function_name, int line_no, const char* tag, const char* msg, va_list prm);

enum NXMessageVerbosity {
        NX_MESSAGE_INFORMATIVE = 0,
        NX_MESSAGE_NORMAL = 1,
        NX_MESSAGE_CAUTIOUS = 2,
        NX_MESSAGE_SILENT = 3
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

void nx_message_verbosity (enum NXMessageVerbosity verb); /**< Sets which messages get printed and which are suppressed. */
void nx_message_fatal_func(__NX_NO_RETURN_PTR NXFatalFuncP fatal_func);      /**< Change the function that gets called in case of a fatal error. */

__NX_END_DECL

#endif

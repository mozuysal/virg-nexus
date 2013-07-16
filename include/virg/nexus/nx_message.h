/**
 * @file nx_message.h
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2013 Izmir Institute of Technology. All rights reserved.
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

typedef __NX_NO_RETURN_PTR void(*NXFatalFuncP)(const char* tag, const char* msg, va_list prm);

enum NXMessageVerbosity {
        NX_MESSAGE_SILENT = -1,
        NX_MESSAGE_CAUTIOUS = 0,
        NX_MESSAGE_NORMAL,
        NX_MESSAGE_INFORMATIVE
};

void nx_info   (const char *tag, const char *msg, ...) __attribute__((format (printf, 2, 3))); /**< Information message that prints only when verbosity level is Informative. */
void nx_log    (const char *tag, const char *msg, ...) __attribute__((format (printf, 2, 3))); /**< Log message that prints only when verbosity level is equal to or above Normal. */
void nx_warning(const char *tag, const char *msg, ...) __attribute__((format (printf, 2, 3))); /**< Warning message that prints only when verbosity level is equal to or above Cautious. */
void nx_error  (const char *tag, const char *msg, ...) __attribute__((format (printf, 2, 3))); /**< Error message that prints gets suppressed only when verbosity level is Silent. */
__NX_NO_RETURN void nx_fatal(const char *tag, const char *msg, ...) __attribute__((format (printf, 2, 3))); /**< Fatal error that should result in an exit() call. */

void nx_vinfo   (const char *tag, const char *msg, va_list prm); /**< Information message that prints only when verbosity level is Informative. */
void nx_vlog    (const char *tag, const char *msg, va_list prm); /**< Log message that prints only when verbosity level is equal to or above Normal. */
void nx_vwarning(const char *tag, const char *msg, va_list prm); /**< Warning message that prints only when verbosity level is equal to or above Cautious. */
void nx_verror  (const char *tag, const char *msg, va_list prm); /**< Error message that prints gets suppressed only when verbosity level is Silent. */
__NX_NO_RETURN void nx_vfatal(const char *tag, const char *msg, va_list prm); /**< Fatal error that should result in an exit() call. */

void nx_message_verbosity (enum NXMessageVerbosity verb); /**< Sets which messages get printed and which are suppressed. */
void nx_message_fatal_func(__NX_NO_RETURN_PTR NXFatalFuncP fatal_func);      /**< Change the function that gets called in case of a fatal error. */

__NX_END_DECL

#endif

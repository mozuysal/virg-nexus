/**
 * @file nx_message_c_impl.c
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
#include "virg/nexus/nx_message.h"

#define _POSIX_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>

#include "virg/nexus/nx_types.h"

#define MESSAGE_STREAM stderr

#define NX_INFO_COLOR_CODE "\x1b[37m"
#define NX_LOG_COLOR_CODE "\x1b[36m"
#define NX_WARNING_COLOR_CODE "\x1b[33m"
#define NX_ERROR_COLOR_CODE "\x1b[31;1m"
#define NX_FATAL_COLOR_CODE "\x1b[35;1m"

#define NX_RESET_COLOR_CODE "\x1b[0m"

static __NX_NO_RETURN void default_fatal_func(const char* function_name, int line_no, const char* tag, const char* msg, va_list prm);

static const char *MESSAGE_HEADER_FORMAT = "[nx%s][%s:%d][%s] ";

static enum NXMessageVerbosity g_message_verbosity = NX_MESSAGE_NORMAL;
static __NX_NO_RETURN_PTR NXFatalFuncP g_fatal_func = default_fatal_func;

static inline void print_call_stack()
{
    const int bt_buffer_sz = 64;
    void *bt_buffer[bt_buffer_sz];

    int bt_sz = backtrace(bt_buffer, bt_buffer_sz);

    char **bt_symbols = backtrace_symbols(bt_buffer, bt_sz);

    fprintf(MESSAGE_STREAM, "Call stack:\n");
    for (int i = 0; i < bt_sz; ++i) {
        fprintf(MESSAGE_STREAM, "   (%2d) %s\n", i, bt_symbols[i]);
    }

    free(bt_symbols);
}

static inline NXBool is_color_terminal()
{
        return isatty(fileno(MESSAGE_STREAM)) != 0;
}

static inline void start_color_output(const char* color_code)
{
        fprintf(MESSAGE_STREAM, "%s", color_code);
}

static inline void stop_color_output()
{
        fprintf(MESSAGE_STREAM, NX_RESET_COLOR_CODE);
}

void default_fatal_func(const char* function_name, int line_no, const char* tag, const char* msg, va_list prm)
{
        NXBool is_color_term = is_color_terminal();
        if (is_color_term) {
                start_color_output(NX_FATAL_COLOR_CODE);
        }
        fprintf(MESSAGE_STREAM, MESSAGE_HEADER_FORMAT, "fatal", function_name, line_no, tag);
        vfprintf(MESSAGE_STREAM, msg, prm);
        if (is_color_term)
                stop_color_output();
        fprintf(MESSAGE_STREAM, "\n");

        print_call_stack();

        exit(NX_EXIT_FATAL);
}

void nx_log_formatted(enum NXLogLevel log_level, const char* function_name, int line_no, const char *tag, const char* msg, ...)
{
        va_list prm;
        va_start(prm, msg);
        if ((int)g_message_verbosity <= (int)log_level) {
                NXBool is_color_term = is_color_terminal();
                if (is_color_term) {
                        switch(log_level) {
                        case NX_LOG_INFO: start_color_output(NX_INFO_COLOR_CODE); break;
                        case NX_LOG_NORMAL: start_color_output(NX_LOG_COLOR_CODE); break;
                        case NX_LOG_WARNING: start_color_output(NX_WARNING_COLOR_CODE); break;
                        case NX_LOG_ERROR: start_color_output(NX_ERROR_COLOR_CODE); break;
                        }
                }
                const char *type_string;
                switch(log_level) {
                case NX_LOG_INFO: type_string = "info"; break;
                case NX_LOG_NORMAL: type_string = "log"; break;
                case NX_LOG_WARNING: type_string = "warning"; break;
                case NX_LOG_ERROR: type_string = "error"; break;
                }
                fprintf(MESSAGE_STREAM, MESSAGE_HEADER_FORMAT, type_string, function_name, line_no, tag);
                vfprintf(MESSAGE_STREAM, msg, prm);
                if (is_color_term)
                        stop_color_output();
                fprintf(MESSAGE_STREAM, "\n");
        }
        va_end(prm);
}

void nx_log_fatal(const char* function_name, int line_no, const char *tag, const char* msg, ...)
{
        va_list prm;
        va_start(prm, msg);
        g_fatal_func(function_name, line_no, tag, msg, prm);
        va_end(prm);
}

void nx_message_verbosity(enum NXMessageVerbosity verb)
{
        g_message_verbosity = verb;
}

void nx_message_fatal_func(NXFatalFuncP fatal_func)
{
        if (fatal_func) {
                g_fatal_func = fatal_func;
        } else {
                g_fatal_func = default_fatal_func;
        }
}

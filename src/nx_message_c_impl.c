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

enum MessageType {
        NX_MT_Info = 0,
        NX_MT_Log,
        NX_MT_Warning,
        NX_MT_Error,
        NX_MT_Fatal
};

#define PRINT_HEADER(type)                                              \
        NXBool is_color_term = is_color_terminal();                     \
        do {                                                            \
                if (is_color_term) {                                    \
                        switch(type) {                                  \
                        case NX_MT_Info: start_color_output(NX_INFO_COLOR_CODE); break; \
                        case NX_MT_Log: start_color_output(NX_LOG_COLOR_CODE); break; \
                        case NX_MT_Warning: start_color_output(NX_WARNING_COLOR_CODE); break; \
                        case NX_MT_Error: start_color_output(NX_ERROR_COLOR_CODE); break; \
                        case NX_MT_Fatal: start_color_output(NX_FATAL_COLOR_CODE); break; \
                        }                                               \
                }                                                       \
                const char *type_string;                                \
                switch(type) {                                          \
                case NX_MT_Info: type_string = "info"; break;           \
                case NX_MT_Log: type_string = "log"; break;             \
                case NX_MT_Warning: type_string = "warning"; break;     \
                case NX_MT_Error: type_string = "error"; break;         \
                case NX_MT_Fatal: type_string = "fatal"; break;         \
                }                                                       \
                fprintf(MESSAGE_STREAM, MESSAGE_HEADER_FORMAT, type_string, tag); \
        } while(0)

#define PRINT_FOOTER()                          \
        do {                                    \
                if (is_color_term)              \
                        stop_color_output();    \
                fprintf(MESSAGE_STREAM, "\n");  \
        } while(0)

static __NX_NO_RETURN void default_fatal_func(const char* tag, const char* msg, va_list prm);

static const char *MESSAGE_HEADER_FORMAT = "[nx%s][%s] ";

static enum NXMessageVerbosity g_message_verbosity = NX_MESSAGE_NORMAL;
static __NX_NO_RETURN_PTR NXFatalFuncP g_fatal_func = default_fatal_func;

static inline void print_call_stack()
{
    const int bt_buffer_sz = 64;
    void *bt_buffer[bt_buffer_sz];

    int bt_sz = backtrace(bt_buffer, bt_buffer_sz);

    char **bt_symbols = backtrace_symbols(bt_buffer, bt_sz);

    fprintf(stderr, "Call stack:\n");
    for (int i = 0; i < bt_sz; ++i) {
        fprintf(stderr, "   (%2d) %s\n", i, bt_symbols[i]);
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

void default_fatal_func(const char* tag, const char* msg, va_list prm)
{
    PRINT_HEADER(NX_MT_Fatal);
    vfprintf(stderr, msg, prm);
    PRINT_FOOTER();

    print_call_stack();

    exit(NX_EXIT_FATAL);
}

void nx_info(const char *tag, const char *msg, ...)
{
        va_list prm;
        va_start(prm, msg);
        nx_vinfo(tag, msg, prm);
        va_end(prm);
}

void nx_log(const char *tag, const char *msg, ...)
{
        va_list prm;
        va_start(prm, msg);
        nx_vlog(tag, msg, prm);
        va_end(prm);
}

void nx_warning(const char *tag, const char *msg, ...)
{
        va_list prm;
        va_start(prm, msg);
        nx_vwarning(tag, msg, prm);
        va_end(prm);
}

void nx_error(const char *tag, const char *msg, ...)
{
        va_list prm;
        va_start(prm, msg);
        nx_verror(tag, msg, prm);
        va_end(prm);
}

void nx_fatal(const char *tag, const char *msg, ...)
{
        va_list prm;
        va_start(prm, msg);
        nx_vfatal(tag, msg, prm);
        va_end(prm);
}

void nx_vinfo(const char *tag, const char *msg, va_list prm)
{
    if (g_message_verbosity > NX_MESSAGE_NORMAL) {
        PRINT_HEADER(NX_MT_Info);
        vfprintf(MESSAGE_STREAM, msg, prm);
        PRINT_FOOTER();
    }
}

void nx_vlog(const char *tag, const char *msg, va_list prm)
{
    if (g_message_verbosity > NX_MESSAGE_CAUTIOUS) {
        PRINT_HEADER(NX_MT_Log);
        vfprintf(MESSAGE_STREAM, msg, prm);
        PRINT_FOOTER();
    }
}

void nx_vwarning(const char *tag, const char *msg, va_list prm)
{
    if (g_message_verbosity > NX_MESSAGE_SILENT) {
        PRINT_HEADER(NX_MT_Warning);
        vfprintf(MESSAGE_STREAM, msg, prm);
        PRINT_FOOTER();
    }
}

void nx_verror(const char *tag, const char *msg, va_list prm)
{
    PRINT_HEADER(NX_MT_Error);
    vfprintf(MESSAGE_STREAM, msg, prm);
    PRINT_FOOTER();
}

void nx_vfatal(const char *tag, const char *msg, va_list prm)
{
        g_fatal_func(tag, msg, prm);
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

/**
 * @file nx_message_c_impl.c
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
#include "virg/nexus/nx_message.h"

#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>

#define MESSAGE_STREAM stderr

#define PRINT_HEADER(type) \
        do {                                                            \
                fprintf(MESSAGE_STREAM, MESSAGE_HEADER_FORMAT, (type), tag);   \
        } while(0)

static __NX_NO_RETURN void default_fatal_func(const char* tag, const char* msg, va_list prm);

static const char *MESSAGE_HEADER_FORMAT = "[%-7s][%s] ";

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

void default_fatal_func(const char* tag, const char* msg, va_list prm)
{
    PRINT_HEADER("fatal");
    vfprintf(stderr, msg, prm);
    fprintf(stderr, "\n\n");

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
        PRINT_HEADER("info");
        vfprintf(MESSAGE_STREAM, msg, prm);
        fprintf(MESSAGE_STREAM, "\n");
    }
}

void nx_vlog(const char *tag, const char *msg, va_list prm)
{
    if (g_message_verbosity > NX_MESSAGE_CAUTIOUS) {
        PRINT_HEADER("log");
        vfprintf(MESSAGE_STREAM, msg, prm);
        fprintf(MESSAGE_STREAM, "\n");
    }
}

void nx_vwarning(const char *tag, const char *msg, va_list prm)
{
    if (g_message_verbosity > NX_MESSAGE_SILENT) {
        PRINT_HEADER("warning");
        vfprintf(MESSAGE_STREAM, msg, prm);
        fprintf(MESSAGE_STREAM, "\n");
    }
}

void nx_verror(const char *tag, const char *msg, va_list prm)
{
    PRINT_HEADER("error");
    vfprintf(MESSAGE_STREAM, msg, prm);
    fprintf(MESSAGE_STREAM, "\n");
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

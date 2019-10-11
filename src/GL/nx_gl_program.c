/**
 * @file nx_gl_program.c
 *
 * Copyright (C) 2019 Mustafa Ozuysal. All rights reserved.
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
 * You should have received a copy of the GNU General Public License
 * along with Foobar.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/GL/nx_gl_program.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_log.h"


GLuint nx_gl_program_xcreate_and_link(GLuint v_shader, GLuint f_shader)
{
        GLuint program = glCreateProgram();
        if (program == 0) {
                NX_FATAL(NXGL_LOG_TAG, "Failed to create OpenGL program");
        }

        glAttachShader(program, v_shader);
        glAttachShader(program, f_shader);
        glLinkProgram(program);

        GLint stat;
        glGetProgramiv(program, GL_LINK_STATUS, &stat);
        if (stat) {
                NX_INFO(NXGL_LOG_TAG, "OpenGL program linked successfully");
        } else {
                NX_ERROR(NXGL_LOG_TAG, "OpenGL program link failed!");
                GLint log_len = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

                char *log = NULL;
                if( log_len > 1 ) {
                        log = NX_NEW_C(log_len);
                        glGetProgramInfoLog(program, log_len, NULL, log);
                        log[log_len-1] = '\0';
                } else {
                        log = NX_NEW_C(1);
                        log[0] = '\0';
                }
                NX_FATAL(NXGL_LOG_TAG, "OpenGL program info log:\n%s",
                         log);
        }

        return program;
}

void nx_gl_program_xvalidate(GLuint program)
{
        NX_ASSERT(program != 0);

        glValidateProgram(program);

        GLint stat;
        glGetProgramiv(program, GL_VALIDATE_STATUS, &stat);
        if (stat) {
                NX_INFO(NXGL_LOG_TAG, "OpenGL program validated successfully");
        } else {
                NX_ERROR(NXGL_LOG_TAG, "OpenGL program validation failed!");
                GLint log_len = 0;
                glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);

                char *log = NULL;
                if( log_len > 1 ) {
                        log = NX_NEW_C(log_len);
                        glGetProgramInfoLog(program, log_len, NULL, log);
                        log[log_len-1] = '\0';
                } else {
                        log = NX_NEW_C(1);
                        log[0] = '\0';
                }
                NX_FATAL(NXGL_LOG_TAG, "OpenGL program info log:\n%s",
                         log);
        }
}

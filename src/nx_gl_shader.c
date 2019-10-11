/**
 * @file nx_gl_shader.c
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
#include "virg/nexus/nx_gl_shader.h"

#include <stdlib.h>

#include "virg/nexus/nx_log.h"
#include "virg/nexus/nx_alloc.h"

const char *nx_shader_type_string(GLenum shader_type)
{
        switch (shader_type) {
        case GL_VERTEX_SHADER:   return "vertex shader";
        case GL_FRAGMENT_SHADER: return "fragment shader";
        default:
                return "unknown shader";
        }
}

GLuint nx_gl_shader_xcreate_and_compile(GLenum shader_type,
                                        const char *shader_source)
{
        GLuint shader = glCreateShader(shader_type);
        if (shader == 0) {
                NX_FATAL(NX_LOG_TAG, "Failed to create OpenGL %s",
                         nx_shader_type_string(shader_type));
        }

        glShaderSource(shader, 1, &shader_source, NULL);

        glCompileShader(shader);
        GLint stat;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &stat);
        if (stat) {
                NX_INFO(NX_LOG_TAG, "OpenGL %s compiled successfully",
                        nx_shader_type_string(shader_type));
        } else {
                NX_ERROR(NX_LOG_TAG, "OpenGL %s compilation failed!",
                         nx_shader_type_string(shader_type));
                GLint log_len = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
                char *log = NULL;
                if( log_len > 1 ) {
                        log = NX_NEW_C(log_len);
                        glGetShaderInfoLog(shader, log_len, NULL, log);
                        log[log_len-1] = '\0';
                } else {
                        log = NX_NEW_C(1);
                        log[0] = '\0';
                }
                NX_FATAL(NX_LOG_TAG, "OpenGL %s compilation log:\n%s",
                         nx_shader_type_string(shader_type), log);
        }

        return shader;
}

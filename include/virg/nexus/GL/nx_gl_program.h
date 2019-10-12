/**
 * @file nx_gl_program.h
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
#ifndef VIRG_NEXUS_NX_GL_PROGRAM_H
#define VIRG_NEXUS_NX_GL_PROGRAM_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/GL/nx_gl.h"
#include "virg/nexus/GL/nx_gl_shader.h"

__NX_BEGIN_DECL

GLuint nx_gl_program_xcreate_and_link(GLuint v_shader, GLuint f_shader);
void   nx_gl_program_xvalidate(GLuint program);

static inline GLuint nx_gl_program_from_vf_shaders(const char *v_shader_src,
                                                   const char *f_shader_src)
{
        GLuint v_shader = nx_gl_shader_xcreate_and_compile(GL_VERTEX_SHADER,
                                                           v_shader_src);
        GLuint f_shader = nx_gl_shader_xcreate_and_compile(GL_FRAGMENT_SHADER,
                                                           f_shader_src);

        GLuint program = nx_gl_program_xcreate_and_link(v_shader, f_shader);
        nx_gl_program_xvalidate(program);

        glDeleteShader(v_shader);
        glDeleteShader(f_shader);

        return program;
}


__NX_END_DECL

#endif

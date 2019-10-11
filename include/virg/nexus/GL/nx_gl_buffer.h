/**
 * @file nx_gl_buffer.h
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
#ifndef VIRG_NEXUS_NX_GL_BUFFER_H
#define VIRG_NEXUS_NX_GL_BUFFER_H

#include "virg/nexus/nx_config.h"
#include "virg/nexus/GL/nx_gl.h"

__NX_BEGIN_DECL

struct NXGLBuffer {
        GLuint id;
        GLenum target;
        GLsizeiptr size;
        const GLvoid *data;
        GLenum usage_hint;
};

struct NXGLBuffer *nx_gl_buffer_alloc();
struct NXGLBuffer *nx_gl_buffer_new(GLenum target, GLsizeiptr size,
                                    const GLvoid *data, GLenum usage_hint);
static inline struct NXGLBuffer *nx_gl_buffer_new_array(GLsizeiptr size,
                                                        const GLvoid *data,
                                                        GLenum usage_hint)
{ return nx_gl_buffer_new(GL_ARRAY_BUFFER, size, data, usage_hint); }

static inline struct NXGLBuffer *nx_gl_buffer_new_element_array(GLsizeiptr size,
                                                                const GLvoid *data,
                                                                GLenum usage_hint)
{ return nx_gl_buffer_new(GL_ELEMENT_ARRAY_BUFFER, size, data, usage_hint); }

void nx_gl_buffer_free(struct NXGLBuffer *buffer);

void nx_gl_buffer_bind(struct NXGLBuffer *buffer);

__NX_END_DECL

#endif

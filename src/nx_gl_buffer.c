/**
 * @file nx_gl_buffer.c
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
#include "virg/nexus/nx_gl_buffer.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_log.h"

struct NXGLBuffer *nx_gl_buffer_alloc()
{
        struct NXGLBuffer *buffer = NX_NEW(1, struct NXGLBuffer);
        glGenBuffers(1, &buffer->id);
        if (buffer->id == 0) {
                NX_FATAL(NX_LOG_TAG, "OpenGL buffer allocation failed!");
        }

        buffer->target = GL_ARRAY_BUFFER;
        buffer->size = 0;
        buffer->data = NULL;
        buffer->usage_hint = GL_STATIC_DRAW;

        return buffer;
}

struct NXGLBuffer *nx_gl_buffer_new(GLenum target, GLsizeiptr size,
                                    const GLvoid *data, GLenum usage_hint)
{
        NX_ASSERT_PTR(data);

        struct NXGLBuffer *buffer = nx_gl_buffer_alloc();

        buffer->target = target;
        buffer->size = size;
        buffer->data = data;
        buffer->usage_hint = usage_hint;

        glBindBuffer(buffer->target, buffer->id);
        glBufferData(buffer->target, buffer->size,
                     buffer->data, buffer->usage_hint);

        return buffer;
}

void nx_gl_buffer_free(struct NXGLBuffer *buffer)
{
        if (buffer) {
                glBindBuffer(buffer->target, 0);
                glDeleteBuffers(1, &buffer->id);
                nx_free(buffer);
        }
}

void nx_gl_buffer_bind(struct NXGLBuffer *buffer)
{
        NX_ASSERT_PTR(buffer);

        glBindBuffer(buffer->target, buffer->id);
}


/**
 * @file nx_gl.c
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
#include "virg/nexus/nx_gl.h"

#include "virg/nexus/nx_log.h"

void nx_gl_info()
{
        const GLubyte* version_str = glGetString(GL_VERSION);
        NX_INFO(NX_LOG_TAG, "%s", (const char *)version_str);

        const GLubyte* sl_version_str = glGetString(GL_SHADING_LANGUAGE_VERSION);
        NX_INFO(NX_LOG_TAG, "%s", (const char *)sl_version_str);
}

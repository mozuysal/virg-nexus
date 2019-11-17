/**
 * @file nx_filesystem.c
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
#include "virg/nexus/nx_filesystem.h"

#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include "virg/nexus/nx_io.h"
#include "virg/nexus/nx_assert.h"

struct NXStringArray *nx_scan_dir(const char *path)
{
        NX_ASSERT_PTR(path);

        struct NXStringArray *dir_contents = nx_string_array_alloc();
        struct dirent **entries;
        int n = scandir(path, &entries, NULL, alphasort);
        if (n < 0) {
                NX_IO_FATAL(NX_LOG_TAG, "Error scanning entries in directory %s", path);
        } else {
                nx_string_array_resize(dir_contents, n);
                for (int i = 0; i < n; ++i) {
                        nx_string_array_set(dir_contents, i, entries[i]->d_name);
                        free(entries[i]);
                }
                free(entries);
                entries = NULL;
        }

        return dir_contents;
}

NXBool nx_check_dir(const char *path)
{
        NX_ASSERT_PTR(path);

        DIR *dir = opendir(path);
        if (!dir || closedir(dir)) {
                return NX_FALSE;
        }

        return NX_TRUE;
}

NXBool nx_check_file(const char *filepath)
{
        NX_ASSERT_PTR(filepath);

        FILE *fin = fopen(filepath, "rb");

        if (fin) {
                nx_fclose(fin, filepath);
                return NX_TRUE;
        } else {
                return NX_FALSE;
        }
}

void nx_ensure_dir(const char *path)
{
        NX_ASSERT_PTR(path);

        if (nx_check_dir(path))
                return;

        if (0 != mkdir(path, 0700)) {
                NX_FATAL(NX_LOG_TAG, "Could not create directory %s", path);
        }
}

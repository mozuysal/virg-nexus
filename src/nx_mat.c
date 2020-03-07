/**
 * @file nx_mat.c
 *
 * Copyright (C) 2020 Mustafa Ozuysal. All rights reserved.
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
 * You should have received a copy of the GNU General Public License along with
 * the VIRG-Nexus Library.  If not, see <https://www.gnu.org/licenses/>.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_mat.h"

#include "virg/nexus/nx_alloc.h"
#include "virg/nexus/nx_assert.h"
#include "virg/nexus/nx_io.h"

void nx_dmat_xwrite(FILE *stream, int m, int n, const double *A)
{
        NX_ASSERT_PTR(stream);
        NX_ASSERT(m >= 0);
        NX_ASSERT(n >= 0);
        NX_ASSERT_PTR(A);

        for (int r = 0; r < m; ++r) {
                for (int c = 0; c < n; ++c) {
                        double e = A[c * m + r];
                        fprintf(stream, "%.17g", e);
                        if (c == n-1)
                                fprintf(stream, "\n");
                        else
                                fprintf(stream, " ");
                }
        }
}

double *nx_dmat_xread(FILE *stream, int *m, int *n)
{
        NX_ASSERT_PTR(stream);
        NX_ASSERT_PTR(m);
        NX_ASSERT_PTR(n);

        size_t sz = 0;
        size_t cap = 9;
        double *A = NX_NEW_D(cap);

        *m = 0;
        *n = 0;

        size_t llen = 256;
        char *line = NX_NEW_C(llen);
        ssize_t n_read = 1;
        while (n_read > 0) {
                int ml = 0;
                n_read = nx_getline(&line, &llen, stream);
                if (n_read > 0) {
                        char *dbegin = line;
                        char *endptr = line;
                        while (NX_TRUE) {
                                double d = strtod(dbegin, &endptr);
                                if (endptr == dbegin)
                                        break;

                                dbegin = endptr;
                                ml++;
                                if (cap <= sz) {
                                        cap *= 2;
                                        A = nx_xrealloc(A, cap * sizeof(*A));
                                }
                                A[sz++] = d;
                        }

                        if (*n == 0) {
                                *n = 1;
                                *m = ml;
                        } else {
                                if (*m != ml) {
                                        NX_FATAL(NX_LOG_TAG, "Reading matrix failed: Expected %d elements on line %d, found %d!",
                                                 *m, *n + 1, ml);
                                }
                                *n += 1;
                        }
                }
        }
        free(line);

        if (*m == 0 || *n == 0) {
                *m = 0;
                *n = 0;
                free(A);
                return NULL;
        }

        nx_dmat_transpose(*m, *n, A);
        int t = *m;
        *m = *n;
        *n = t;
        return A;
}

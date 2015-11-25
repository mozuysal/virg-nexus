/**
 * @file nx_data_frame.c
 *
 * This file is part of the IYTE Visual Intelligence Research Group Software Library
 *
 * Copyright (C) 2015 Mustafa Ozuysal. All rights reserved.
 *
 * @author Mustafa Ozuysal
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "virg/nexus/nx_data_frame.h"

#include "virg/nexus/nx_alloc.h"

struct NXDataFrame {
};

struct NXDataFrame *nx_data_frame_alloc()
{
        struct NXDataFrame *df = NX_NEW(1, struct NXDataFrame);

        return df;
}

void nx_data_frame_free(struct NXDataFrame *df)
{
        if (df != NULL) {
                nx_free(df);
        }
}

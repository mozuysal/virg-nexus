/**
 * @file nx_keypoint.c
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
#include "virg/nexus/nx_keypoint.h"

#include "virg/nexus/nx_io.h"

#define WRITE_FIELD(arr,i,field,stream) do {                                  \
                if (fwrite(&arr[i].field, sizeof(arr[i].field), 1, stream) != 1) \
                        return i;                                       \
        } while (0)

#define READ_FIELD(arr,i,field,stream) do {                             \
                if (fread(&arr[i].field, sizeof(arr[i].field), 1, stream) != 1) \
                        return i;                                       \
        } while (0)

int nx_keypoint_write(const struct NXKeypoint *key, int n, FILE *stream)
{
        for (int i = 0; i < n; ++i) {
                WRITE_FIELD(key,i,x,stream);
                WRITE_FIELD(key,i,y,stream);
                WRITE_FIELD(key,i,xs,stream);
                WRITE_FIELD(key,i,ys,stream);
                WRITE_FIELD(key,i,level,stream);
                WRITE_FIELD(key,i,scale,stream);
                WRITE_FIELD(key,i,sigma,stream);
                WRITE_FIELD(key,i,score,stream);
                WRITE_FIELD(key,i,ori,stream);
                WRITE_FIELD(key,i,id,stream);
        }

        return n;
}

int nx_keypoint_read(struct NXKeypoint *key, int n, FILE *stream)
{
        for (int i = 0; i < n; ++i) {
                READ_FIELD(key,i,x,stream);
                READ_FIELD(key,i,y,stream);
                READ_FIELD(key,i,xs,stream);
                READ_FIELD(key,i,ys,stream);
                READ_FIELD(key,i,level,stream);
                READ_FIELD(key,i,scale,stream);
                READ_FIELD(key,i,sigma,stream);
                READ_FIELD(key,i,score,stream);
                READ_FIELD(key,i,ori,stream);
                READ_FIELD(key,i,id,stream);
        }

        return n;
}

void nx_keypoint_xwrite(const struct NXKeypoint *key, int n, FILE *stream)
{
        int n_wr = nx_keypoint_write(key, n, stream);
        if (n_wr != n)
                NX_IO_FATAL(NX_LOG_TAG, "Error writing keypoints, wrote %d of %d", n_wr, n);
}

void nx_keypoint_xread(struct NXKeypoint *key, int n, FILE *stream)
{
        int n_rd = nx_keypoint_read(key, n, stream);
        if (n_rd != n)
                NX_IO_FATAL(NX_LOG_TAG, "Error reading keypoints, read %d of %d", n_rd, n);
}



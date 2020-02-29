/**
 * @file fast_nonmax.c
 *
 * Copyright (C) 2019,2020 Mustafa Ozuysal. All rights reserved.
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
 * Modified by Mustafa Ozuysal @ 2013
 *
 * Takes buffers for scores and keypoints instead of allocating and releasing
 * them.
 *
 * Contact mustafaozuysal@iyte.edu.tr for comments and bug reports.
 *
 */
#include "stdlib.h"

#include "virg/nexus/nx_keypoint.h"

#define Compare(X, Y) ((X)>=(Y))

void fast_nonmax_suppression(int *ret_num_nonmax, struct NXKeypoint *ret_nonmax, int num_corners, const struct NXKeypoint *corners);

void fast_nonmax_suppression(int *ret_num_nonmax, struct NXKeypoint *ret_nonmax, int num_corners, const struct NXKeypoint *corners)
{
	int num_nonmax=0;
	int last_row;
	int* row_start;
	int i, j;
	const int sz = (int)num_corners;

	/*Point above points (roughly) to the pixel above the one of interest, if there
          is a feature there.*/
	int point_above = 0;
	int point_below = 0;

	if (num_corners < 1 || *ret_num_nonmax <= 0)
	{
		*ret_num_nonmax = 0;
		return;
	}

	/* Find where each row begins
	   (the corners are output in raster scan order). A beginning of -1 signifies
	   that there are no corners on that row. */
	last_row = corners[num_corners-1].y;
	row_start = (int*)malloc((last_row+1)*sizeof(int));

	for(i=0; i < last_row+1; i++)
		row_start[i] = -1;

	{
		int prev_row = -1;
		for(i=0; i< num_corners; i++)
			if(corners[i].y != prev_row)
			{
				row_start[corners[i].y] = i;
				prev_row = corners[i].y;
			}
	}



	for(i=0; i < sz; i++)
	{
		int score = corners[i].score;
		const struct NXKeypoint *pos = corners + i;

		/*Check left */
		if(i > 0)
			if(corners[i-1].x == pos->x-1 && corners[i-1].y == pos->y && Compare(corners[i-1].score, score))
				continue;

		/*Check right*/
		if(i < (sz - 1))
			if(corners[i+1].x == pos->x+1 && corners[i+1].y == pos->y && Compare(corners[i+1].score, score))
				continue;

		/*Check above (if there is a valid row above)*/
		if(pos->y != 0 && row_start[pos->y - 1] != -1)
		{
			/*Make sure that current point_above is one
			  row above.*/
			if(corners[point_above].y < pos->y - 1)
				point_above = row_start[pos->y-1];

			/*Make point_above point to the first of the pixels above the current point,
			  if it exists.*/
			for(; corners[point_above].y < pos->y && corners[point_above].x < pos->x - 1; point_above++)
			{}


			for(j=point_above; corners[j].y < pos->y && corners[j].x <= pos->x + 1; j++)
			{
				int x = corners[j].x;
				if( (x == pos->x - 1 || x ==pos->x || x == pos->x+1) && Compare(corners[j].score, score))
					goto cont;
			}

		}

		/*Check below (if there is anything below)*/
		if(pos->y != last_row && row_start[pos->y + 1] != -1 && point_below < sz) /*Nothing below*/
		{
			if(corners[point_below].y < pos->y + 1)
				point_below = row_start[pos->y+1];

			/* Make point below point to one of the pixels belowthe current point, if it
			   exists.*/
			for(; point_below < sz && corners[point_below].y == pos->y+1 && corners[point_below].x < pos->x - 1; point_below++)
			{}

			for(j=point_below; j < sz && corners[j].y == pos->y+1 && corners[j].x <= pos->x + 1; j++)
			{
				int x = corners[j].x;
				if( (x == pos->x - 1 || x ==pos->x || x == pos->x+1) && Compare(corners[j].score,score))
					goto cont;
			}
		}

		ret_nonmax[num_nonmax++] = corners[i];
                if (num_nonmax == *ret_num_nonmax)
                        break;
        cont:
                ;
	}

	free(row_start);
	*ret_num_nonmax = num_nonmax;
}

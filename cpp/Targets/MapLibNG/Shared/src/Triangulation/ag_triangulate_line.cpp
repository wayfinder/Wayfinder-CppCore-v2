/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "Triangulation/ag_triangulate.h"

#include <cstdlib>
#include <limits.h>

/**
 * Changes x and y so that a perpendicular vector is returned.
 */
#define MAKE_PERPENDICULAR(x,y,tmp_var)         \
	do {                                         \
		(tmp_var) = -(x);                         \
		(x) = (y);                                \
		(y) = (tmp_var);                          \
	} while(0)

/**
 * Sets x_new and y_new to the quadrant that x_org and y_org are in.
 */
#define SET_QUADRANT(x_org, y_org, x_new, y_new)   \
	do {                                            \
		if ((x_org) < 0) (x_new) = -(x_new);         \
		if ((y_org) < 0) (y_new) = -(y_new);         \
	} while (0)

/**
 * Sets the vector (x,y) (assumed to be of length 128) to the length width/2
 */
#define SET_WIDTH(x, y, width)                  \
	do {                                         \
		(x) = ((x)*(width)) >> 8;                 \
		(y) = ((y)*(width)) >> 8;                 \
	} while (0)


static unsigned char intsine[16][16] = {
   { 91,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0 },
   {128, 91, 57, 40, 31, 25, 21, 18, 16, 14, 13, 12, 11, 10,  9,  9 },
   {128,114, 91, 71, 57, 48, 40, 35, 31, 28, 25, 23, 21, 19, 18, 17 },
   {128,121,107, 91, 77, 66, 57, 50, 45, 40, 37, 34, 31, 29, 27, 25 },
   {128,124,114,102, 91, 80, 71, 64, 57, 52, 48, 44, 40, 38, 35, 33 },
   {128,126,119,110,100, 91, 82, 74, 68, 62, 57, 53, 49, 46, 43, 40 },
   {128,126,121,114,107, 98, 91, 83, 77, 71, 66, 61, 57, 54, 50, 48 },
   {128,127,123,118,111,104, 97, 91, 84, 79, 73, 69, 64, 61, 57, 54 },
   {128,127,124,120,114,109,102, 96, 91, 85, 80, 75, 71, 67, 64, 60 },
   {128,127,125,121,117,112,107,101, 96, 91, 86, 81, 77, 73, 69, 66 },
   {128,127,126,123,119,114,110,105,100, 95, 91, 86, 82, 78, 74, 71 },
   {128,127,126,123,120,117,112,108,104, 99, 95, 91, 86, 83, 79, 76 },
   {128,128,126,124,121,118,114,111,107,102, 98, 94, 91, 87, 83, 80 },
   {128,128,127,125,122,119,116,113,109,105,101, 98, 94, 91, 87, 84 },
   {128,128,127,125,123,121,118,114,111,108,104,101, 97, 94, 91, 87 },
   {128,128,127,126,124,121,119,116,113,110,107,103,100, 97, 94, 91}};

/**
 * Given a vector (x,y) in the first quadrant, returns one of roughly 90
 * possible approximate vectors of length 128.
 *
 * @param[in]	x		x coordinate
 * @param[in]	y		y coordinate
 * @param[out] 	vx		Approximate x coordinate
 * @param[out]	vy		Approximate y coordinate
 */
static inline void line2wvec(unsigned int x, unsigned int y,
                             pal::wf_int32 *vx, pal::wf_int32 *vy) {
   unsigned int t;
   int swap = 0;
   if (x > y) {
      t = x;
      x = y;
      y = t;
      swap = 1;
   }

   t = y >> 4;
   while (t) {
      t >>= 1;
      x >>= 1;
      y >>= 1;
   }
   if (swap) {
      *vy = intsine[x][y];
      *vx = intsine[y][x];
   }
   else {
      *vx = intsine[x][y];
      *vy = intsine[y][x];
   }
}

/**
 * Outputs a point
 *
 * @param[in]	x		x coordinate
 * @param[in]	y		y coordinate
 * @param[out]	out		Out array
 */
static inline void output_point(int x, int y, struct ag_point *out) {
	out->x = x;
	out->y = y;
}

/**
 * Returns the difference in x and y between two points given as *pnts and
 * *(pnts+1).
 *
 * @param[in]	pnts	An array of points. The function will calculate the
 * 						difference between *pnts and *(pnts+1)
 * @param[out]	x_diff	The difference in x
 * @param[out]	y_diff	The difference in y
 */
static inline void get_diff_vector(const struct ag_point *pnts, pal::wf_int32 *x_diff,
                                   pal::wf_int32 *y_diff) {
	*x_diff = pnts[1].x - pnts[0].x;
	*y_diff = pnts[1].y - pnts[0].y;
}


/**
 * Premultiplies cosLat with the correct value and casts it to int.
 *
 * @param[in]	coslat	The current cosLat
 * @return				The scaled cosLat or INT_MAX on overflow
 */
int coslat2int(float cosLat) {
	cosLat *= (1 << COSLAT_UP_SHIFT);
	if (cosLat > INT_MAX) return INT_MAX;
	else return (int)cosLat;
}


/**
 * Returns the value (1/cosLat) scaled with the correct value as an integer. If
 * cosLat is small enough to give overflow (< 0.000061 for COSLAT_UP_SHIFT =
 * 18), returns INT_MAX.
 *
 * @param[in]	cosLat	The current cosLat
 * @return				(1/cosLat) correctly scaled or INT_MAX on overflow
 */
int coslat2coslatinv(float coslat) {
	coslat = 1/coslat;
	coslat *= (1 << COSLAT_UP_SHIFT);
	if (coslat > INT_MAX) return INT_MAX;
	else return (int)coslat;
}


/**
 * Triangulates a line given by n>1 points with a given width.
 *
 * Notes on overflow:
 * In order to avoid overflow, cosLat (the original floating point value)
 * cannot be less than 1/(32-COSLAT_UP_SHIFT). For COSLAT_UP_SHIFT set to 18,
 * this corresponds to cosLat >~ 0.000061.
 * If the input points have y sufficiently close to minimum or maximum int,
 * overflow might occur, but since this corresponds to the north/south-pole, it
 * should not be a problem.
 *
 * @param[in]	n		The number of points in the line
 * @param[in]	pnts	n coordinate points
 * @param[in]	cosLat	The cosLat value as returned by coslat2int()
 * @param[in]	cosLat_inv
 The inverse cosLat value as returned by
 coslat2coslatinv()
 * @param[in]	width	The width of the line
 * @param[out]	out		The output, 4*n coordinate points.
 * @return				1 if successful, 0 if not (only unsuccessful if n < 2)
 */
int triangulate_line(int n, const struct ag_point *pnts, int cosLat,
                     int cosLat_inv, int width, struct ag_point *out) {

	struct ag_point *out_ind = out;
	pal::wf_int32 diff_x, diff_y, cur_x, cur_y, i, tmp;
	pal::wf_int64 tmp64;

	if (n < 2) {
		return 0;
	}

	// Begin cap, consists of two points "behind" the starting point

	// Find direction vector between reference points
	get_diff_vector(&pnts[0], &diff_x, &diff_y);

	// Round the direction vector to a known vector with length 128 (in first
	// quadrant)
	line2wvec(abs(diff_x), abs(diff_y), &cur_x, &cur_y);

	// Move approximation vector back to correct quadrant
	SET_QUADRANT(diff_x, diff_y, cur_x, cur_y);

	// Set length of approximation vector to width/2 (= downshift by 8 since
	// approximation vector has length 128)
	SET_WIDTH(cur_x, cur_y, width);

	// Output 2 cap points
	output_point(pnts[0].x - cur_x, pnts[0].y - cur_y, out_ind++);
	*out_ind = *(out_ind-1);
	out_ind++;

	for (i = 0; i < n-1; i++) {
		// Find direction vector
		get_diff_vector(&pnts[i], &diff_x, &diff_y);

		// Map direction vector to output coordinate system (= multiply x by
		// coslat)
		tmp64 = ((pal::wf_int64)diff_x)*cosLat;

		// Approximate vector to a vector in first quadrant
		line2wvec(abs((int)(tmp64 >> COSLAT_UP_SHIFT)), abs(diff_y), &cur_x,
                &cur_y);

		// Move approximation to correct quadrant
		SET_QUADRANT(diff_x, diff_y, cur_x, cur_y);

		// Make perpendicular
		MAKE_PERPENDICULAR(cur_x, cur_y, tmp);

		// Map perpendicular vector back to input coordinate system (= divide x
		// by coslat)

		tmp64 = ((pal::wf_int64)cur_x) * cosLat_inv;
		cur_x = (pal::wf_int32)(tmp64 >> COSLAT_UP_SHIFT);

		// Set correct length
		SET_WIDTH(cur_x, cur_y, width);

		// Output
		output_point(pnts[i].x + cur_x, pnts[i].y + cur_y, out_ind++);
		output_point(pnts[i].x - cur_x, pnts[i].y - cur_y, out_ind++);
		output_point(pnts[i+1].x + cur_x, pnts[i+1].y + cur_y, out_ind++);
		output_point(pnts[i+1].x - cur_x, pnts[i+1].y - cur_y, out_ind++);
	}

	// End cap, consists of two points "after" the last point
	get_diff_vector(&pnts[n-2], &diff_x, &diff_y);

	line2wvec(abs(diff_x), abs(diff_y), &cur_x, &cur_y);

	SET_QUADRANT(diff_x, diff_y, cur_x, cur_y);
	SET_WIDTH(cur_x, cur_y, width);

	// Output 2 cap points
	output_point(pnts[n-1].x + cur_x, pnts[n-1].y + cur_y, out_ind++);
	*out_ind = *(out_ind-1);
	out_ind++;

	return 1;
}


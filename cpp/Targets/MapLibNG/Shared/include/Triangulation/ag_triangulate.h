/*
Copyright (c) 1999 - 2010, Vodafone Group Services Ltd
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of the Vodafone Group Services Ltd nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


// Disable algotrim functionality completely.

#include "PALMachineTypes.h"

#ifndef TRIANGULATE_H_
#define TRIANGULATE_H_

/**
 * Defines the value (1 << COSLAT_UP_SHIFT) that coslat should be premultiplied
 * with before being used by triangulate_line. A minimum value of 17 is needed
 * to ensure good precision. Larger values may give overflow if cosLat is very
 * small. (A value of 18 requires cosLat to be larger than 1/(32-18) ~ 0.00006)
 */
#define COSLAT_UP_SHIFT 18


struct ag_point {
   pal::wf_int32 y, x;
};


/**
 * Premultiplies cosLat with the correct value and casts it to int.
 *
 * @param[in]	coslat	The current cosLat
 * @return				The scaled cosLat
 */
int coslat2int(float cosLat);


/**
 * Returns the value (1/cosLat) scaled with the correct value as an integer. If
 * cosLat is small enough to give overflow (< 0.000061 for COSLAT_UP_SHIFT =
 * 18), returns INT_MAX.
 *
 * @param[in]	cosLat	The current cosLat
 * @return				(1/cosLat) correctly scaled or INT_MAX on overflow
 */
int coslat2coslatinv(float cosLat);


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
                     int cosLat_inv, int width, struct ag_point *out);


/**
 * Triangulates a polygon given in clockwise order
 *
 * @param[in]	n		Number of vertices in polygon
 * @param[in]	pnts	n polygon vertices in clockwise order
 * @param[in]	wmem	Pointer to working memory. Assumed to be at least the
 * 						size returned by get_memory_usage
 * @param[out]	out		3*(n-2) indices into pnts, representing the
 * 						triangulation.
 * @return				1 if successful, 0 if not
 * @sa					get_memory_usage
 */
int triangulate_poly(int n, const struct ag_point *pnts, void *wmem, int *out);


/**
 * Returns the amount of memory required to run triangulate_poly
 *
 * @param[in]	n		The number of vertices
 * @return				The number of bytes needed to run triangulate_poly
 */
unsigned int triangulate_poly_mem_usage(int n);

#endif /* TRIANGULATE_H_ */

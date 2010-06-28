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

#include <algorithm>	// Contains std::sort
#include <limits.h>


// Use for optimizing if-else
#if defined (__GNUC__) 
#define unlikely(expr) __builtin_expect(!!(expr),0)
#define likely(expr) __builtin_expect(!!(expr),1)
#else
#define likely(expr) (expr)
#define unlikely(expr) (expr)
#endif

// A linked list will be stored in an array sorted on y-coordinate.
enum state {CONVEX = -1, UNKNOWN = 0, REFLEX = 1};

struct pelem {
	int index;	///< Vertex index in pnts array, -1 if vertex has been removed
	int next;	///< Index of next vertex in p (counter-clockwise)
	int prev;	///< Index of previous vertex in p (counter-clockwise)
	int state;	///< Vertex state: CONVEX, REFLEX or UNKNOWN
};


// Begin C++ code
/**
 * Comparator for sorting the linked list. Sorts small y before large, and if
 * equal, small x before large. Also finds the min and max of x and y and
 * stores them in max_coord and min_coord. If (max_coord - min_coord) > INT_MAX
 * overflow is a possibility and downshifts in is_reflex is necessary.
 */
struct comparator {
	const struct ag_point *pnts;

	bool operator() (const struct pelem &a, const struct pelem &b) {
		if (pnts[a.index].y < pnts[b.index].y) {
			return true;
		}
		else if (pnts[a.index].y == pnts[b.index].y) {
			return pnts[a.index].x < pnts[b.index].x;
		}
		else {
			return false;
		}
	}
};
// End C++ code


#ifndef NDEBUG
#include <stdio.h>
/**
 * Prints the polygon given by p and pnts in code that can be copy-pasted to
 * matlab for easy viewing.
 */
void dump2matlab(int remaining, int ind, const struct ag_point *pnts,
                 struct pelem *p) {
	int i;
	printf("x = zeros(%d,1); y = zeros(%d,1);\n", remaining, remaining);
	for (i = 1; i <= remaining; i++) {
		printf("x(%d) = %d; y(%d) = %d;\n", i, pnts[p[ind].index].x, i, 
             pnts[p[ind].index].y);
		ind = p[ind].next;
	}
	printf("plot(x, y, 'b-*', [x(end) x(1)], [y(end) y(1)], 'b-*', x(1), y(1),\
			'ro', x(2), y(2), 'go')\n");
}
#endif


/**
 * Check if there are any points inside the given triangle using floating-point
 * barycentric coordinates.
 *
 * The function assumes that state != UNKNOWN for all vertices since is_reflex
 * should have been run for all vertices before desperate mode is used.
 *
 * @param[in]	ind		The given triangle
 * @param[in]	pnts	Vertex coordinate array
 * @param[in]	p		Linked list containing remaining vertices
 * @return				1 if there are no points inside the triangle, else 0
 */
static inline int point_in_triang(struct pelem *p, int i,
                                  const struct ag_point *pnts) {
	int j = i, x, y, x_prev, y_prev, x_cent, y_cent, x_next, y_next;
	float u, v, d, e0x, e1x, e2x, e0y, e1y, e2y;

	do {
		// Only reflex vertices that are not one of the corners of the triangle
		// need to be checked.
		if (p[j].state != REFLEX || j == i ||
          j == p[i].next || j == p[i].prev) {
			j = p[j].prev;
			continue;
		}

		// The current point
		x = pnts[p[j].index].x;
		y = pnts[p[j].index].y;

		// The triangle corners
		x_prev = pnts[p[p[i].prev].index].x;
		y_prev = pnts[p[p[i].prev].index].y;
		x_cent = pnts[p[i].index].x;
		y_cent = pnts[p[i].index].y;
		x_next = pnts[p[p[i].next].index].x;
		y_next = pnts[p[p[i].next].index].y;

		// If the point (x,y) matches any of the corner points, consider it
		// outside the triangle. (Only possible if the polygon contains
		// multiple points with identical coordinates)
		if ((x == x_prev && y == y_prev) ||
          (x == x_cent && y == y_cent) ||
          (x == x_next && y == y_next)) {
			j = p[j].prev;
			continue;
		}

		// Calculate barycentric basis vectors
		e0x = (float)x - x_prev;
		e0y = (float)y - y_prev;
		e1x = (float)x_cent - x_prev;
		e1y = (float)y_cent - y_prev;
		e2x = (float)x_next - x_prev;
		e2y = (float)y_next - y_prev;

		// u and v are the barycentric coordinates
		// If 0<u<1 and 0<v<1 and (u+v)<1, the point is inside the triangle.
		// Special care is taken so that no division by zero is performed
		if (e1x == 0.0f) {
			if (e2x == 0.0f) {
				// Malformed triangle with zero area. No points can be inside
				j = p[j].prev;
				continue;
			}
			u = e0x/e2x;
			if (u < 0.0f || u > 1.0f) {
				j = p[j].prev;
				continue;
			}
			if (e1y == 0.0f) {
				// Malformed triangle with zero area. No points can be inside
				j = p[j].prev;
				continue;
			}
			v = (e0y-e2y*u)/e1y;
			if (v < 0.0f) {
				j = p[j].prev;
				continue;
			}
		}
		else {
			d = e2y*e1x - e2x*e1y;
			if( d == 0.0f ) {
				// Malformed triangle with zero area. No points can be inside
				j = p[j].prev;
				continue;
			}
			u = (e0y*e1x - e0x*e1y) / d;
			if ( u < 0.0f || u > 1.0f) {
				j = p[j].prev;
				continue;
			}
			v = (e0x - e2x*u) / e1x;

			if( v < 0.0f ) {
				j = p[j].prev;
				continue;
			}
		}

		if ((u + v) > 1.0f) {
			j = p[j].prev;
			continue;
		}
		else {
			return 0;
		}
	} while (j != i);
	return 1;
}


/**
 * Finds the minimum and maximum of three input values
 */
#define MINMAX(v1, v2, v3, min, max)            \
	do {                                         \
		(min) = (v1);                             \
		(max) = (v1);                             \
		if ((v2) < (min)) (min) = (v2);           \
		else (max) = (v2);                        \
		if ((v3) < (min)) (min) = (v3);           \
		if ((v3) > (max)) (max) = (v3);           \
	} while (0);

/**
 * Sorts three input values into min, mid and max.
 */
#define MINMEDMAX(v1, v2, v3, min, mid, max)    \
	do {                                         \
		(min) = (v1);                             \
		(mid) = (v1);                             \
		(max) = (v1);                             \
		if ((v2) < (min)) (min) = (v2);           \
		else (max) = (v2);                        \
		if ((v3) < (min)) {                       \
			(mid) = (min);                         \
			(min) = (v3);                          \
		}                                         \
		else if ((v3) > (max)) {                  \
			(mid) = (max);                         \
			(max) = (v3);                          \
		}                                         \
		else {                                    \
			(mid) = (v3);                          \
		}                                         \
	} while (0);


/**
 * Check if there are any points in the bounding box of the given triangle. 
 * This is a fast method using no multiplications or floating point.
 *
 * @param[in]	ind		Vertex index in p. Defines the triangle using left, 
 *						ind and right as vertices
 * @param[in]	pnts	Vertex coordinate array
 * @param[in]	p		Linked list containing remaining vertices
 * @return				1 if there are points in the bounding box, else 0
 */
static inline int point_in_box(struct pelem *p, int i,
                               const struct ag_point *pnts) {
	// Indices are lexicographically sorted, so comparing y indices is equal
	// to comparing their values.
	int yi1 = p[i].next;
	int yi2 = i;
	int yi3 = p[i].prev;
	int x1 = pnts[p[yi1].index].x;
	int x2 = pnts[p[yi2].index].x;
	int x3 = pnts[p[yi3].index].x;
	int min_x, max_x, min_y, mid_y, max_y, j;

	// Determine min, med and max y indices and min and max x coordinates
	MINMAX(x1, x2, x3, min_x, max_x);
	MINMEDMAX(yi1, yi2, yi3, min_y, mid_y, max_y);

	// For a point with index j, it can only be inside the bounding box if
	// min_y < j < max_y. Therefore, only these indices need to be checked.
	// This is true even if index (min_y-1) would have the same y-value as
	// min_y, since it would then have to have a smaller x-value, and can
	// therefore not be inside the box.
	for (j = min_y + 1; j < max_y; j++) {
		// Ignore vertices that have been removed (index < 0), have state
		// CONVEX, or are corners of the triangle (only one corner can be
		// between min_y and max_y)
		if (p[j].index < 0 || p[j].state == CONVEX || j == mid_y) {
			continue;
		}
		if (pnts[p[j].index].x <= max_x && pnts[p[j].index].x >= min_x) {
			return 1;
		}
	}
	return 0;
}


/**
 * Determines if a vertex is convex or reflex. Only convex vertices are valid
 * for removal, and there can only be vertices inside a given triangle if one
 * of them is reflex. Vertices that are on a line (or almost on a line) are
 * given the state convex. This may give overlapping output triangles, but no
 * triangles will have crossing edges.
 *
 * This function basically determines the sign of the cross-product of the
 * vectors (next-current) and (previous-current).
 *
 * @param[in]	i		Vertex index in p
 * @param[in] 	pnts	Vertex coordinate array
 * @param[in]	p		Linked list containing remaining vertices
 * @param[in]	shift_down
 * 						If !=0, coordinates must be downshifted in order to
 * 						guarantee no overflow.
 * @return				REFLEX or CONVEX. Note: If points are on a line or
 *						almost on a line CONVEX is returned.
 */
static inline state is_reflex(struct pelem *p, int i,
                              const struct ag_point *pnts, int shift_down) {
	int x_curr = pnts[p[i].index].x;
	int y_curr = pnts[p[i].index].y;
	int x_next = pnts[p[p[i].next].index].x;
	int y_next = pnts[p[p[i].next].index].y;
	int x_prev = pnts[p[p[i].prev].index].x;
	int y_prev = pnts[p[p[i].prev].index].y;
	pal::wf_int32 diff1, diff2, diff3, diff4;
	pal::wf_int64 mul1, mul2;

	// Perform downshift if coordinates have been found large enough to be able
	// to give overflow.
	if (unlikely(shift_down)) {
		diff1 = (x_next >> 1) - (x_curr >> 1);
		diff2 = (y_prev >> 1) - (y_curr >> 1);
		diff3 = (x_prev >> 1) - (x_curr >> 1);
		diff4 = (y_next >> 1) - (y_curr >> 1);
	}
	else {
		diff1 = x_next - x_curr;
		diff2 = y_prev - y_curr;
		diff3 = x_prev - x_curr;
		diff4 = y_next - y_curr;
	}

	mul1 = ((pal::wf_int64)diff1)*diff2;
	mul2 = ((pal::wf_int64)diff3)*diff4;

	if (mul1 > mul2) {
		return REFLEX;
	}
	else {
		return CONVEX;
	}
}


/**
 * Uses floating point methods to find a removable triangle
 *
 * @param[in,out]	i		The current vertex index, set to a removable vertex
 * 							if one is found
 * @param[in]	remaining	The number of remaining vertices
 * @param[in]	pnts		The vertice coordinates
 * @param[in]	p			The linked list containing the remaining vertices
 * @return					0 if no removable vertex is found, else 1
 */
static inline int find_removable_vertex(struct pelem *p, int *i,
                                        const struct ag_point *pnts, int remaining) {
	int nbr_of_fails = 0, j = *i;
	// Extreme desperation, check points in triangle with floating point
	while (nbr_of_fails < remaining) {
		// Vertex is valid for removal if it has state CONVEX and there are no
		// points inside the triangle consisting of it and its two neighbours.
		if (p[j].state == CONVEX) {
			if (point_in_triang(p, j, pnts)) {
				*i = j;
				return 1;
			}
		}
		j = p[j].prev;
		nbr_of_fails++;
	}
	return 0;
}

/**
 * Initializes and sorts the linked list so that p[0] contains the lowest
 * y-coordinate and p[n-1] contains the greatest. Also checks if there are
 * coordinates large enough to generate overflow.
 *
 * @param[in]		n		The number of vertices
 * @param[in]		pnts	The vertice coordinates
 * @param[in,out]	p		An array containing n list nodes
 * @return					True if max coordinate minus min coordinate is
 * 							larger than INT_MAX, (= overflow is possible)
 */
static inline int pelem_array_init(struct pelem *p, int n,
                                   const struct ag_point *pnts) {
	int i, max_coord = 0, min_coord = 0;
	pal::wf_int64 tmp64;
	comparator comp;
	comp.pnts = pnts;

	for (i = 0; i < n; i++) {
		p[i].index = i;
		p[i].next = i + 1;
		p[i].prev = i - 1;
	}
	p[0].prev = n-1;
	p[n-1].next = 0;

	// Begin C++ code
	std::sort(p, p+n, comp);
	// End C++ code

	for (i = 0; i < n; i++) {
		p[p[i].index].state = i;
	}
	for (i = 0; i < n; i++) {
		p[i].next = p[p[i].next].state;
		p[i].prev = p[p[i].prev].state;
	}
	for (i = 0; i < n; i++) {
		p[i].state = 0;
	}

	// Finds the maximum and minimum coordinates
	for (i = 0; i < n; i++) {
		if (pnts[i].x > pnts[i].y) {
			if (pnts[i].x > max_coord) max_coord = pnts[i].x;
			if (pnts[i].y < min_coord) min_coord = pnts[i].y;
		}
		else {
			if (pnts[i].y > max_coord) max_coord = pnts[i].y;
			if (pnts[i].x < min_coord) min_coord = pnts[i].x;
		}
	}

	// If maximum - minimum is larger than INT_MAX, overflow is possible
	tmp64 = (pal::wf_int64)(max_coord) - min_coord;
	return tmp64 > (pal::wf_int64)INT_MAX;
}

/**
 * Removes and outputs a triangle
 *
 * @param[in]	ind		The triangle to be removed (given by ind and its left 
 *						and right values)
 * @param[in]	pnts	The coordinate array
 * @param[in]	p		The "linked list" containing all remaining vertices
 * @param[out]	q		Pointer to the output pointer.
 */
static inline void remove_triangle(struct pelem *p, int ind,
                                   const struct ag_point *pnts, int **q) {

	// Output triangle
	*(*q)++ = p[p[ind].next].index;
	*(*q)++ = p[ind].index;
	*(*q)++ = p[p[ind].prev].index;

	// Set state to unknown for remaining two vertices in triangle
	p[p[ind].next].state = UNKNOWN;
	p[p[ind].prev].state = UNKNOWN;

	// Remove point
	p[p[ind].next].prev = p[ind].prev;
	p[p[ind].prev].next = p[ind].next;
	p[ind].index = -1;
}

/**
 * Returns the number of triangle indices pushed on the output.
 *
 * @param[in]	out		The output struct.
 * @return				The number of points pushed on the output.
 */
//static inline int output_size(struct output *out) {
//	return out->curr_out - out->start;
//}


/**
 * Returns the amount of memory required to run triangulate_poly
 *
 * @param[in]	n		The number of vertices
 * @return				The number of bytes needed to run triangulate_poly
 */
unsigned int triangulate_poly_mem_usage(int n) {
	return sizeof(pelem)*n;
}


/**
 * Triangulate a polygon given in clockwise order
 *
 * @param[in]	n		Number of vertices in polygon
 * @param[in]	pnts	n polygon vertices in clockwise order
 * @param[in]	wmem	Pointer to working memory. Assumed to be at least the
 * 						size returned by get_memory_usage
 * @param[out]	out		3*(n-2) computed point indices, representing the
 * 						triangulation.
 * @return				1 if successful, 0 if not
 * @sa					get_memory_usage
 */
int triangulate_poly(int n, const struct ag_point *pnts, void *wmem,
                     int *out_start) {
	struct pelem *p = (struct pelem *)wmem;
	int *q = out_start;
	int i = 0, remaining = n, nbr_of_fails = 0, large_coord;

	if (remaining < 2) {
		return 1;
	}
	else if (remaining == 2) {
		*q++ = 0;
		*q++ = 1;
		*q++ = 0;
		return 1;
	}
	else if (remaining == 3) {
		*q++ = 0;
		*q++ = 1;
		*q++ = 2;
		return 1;
	}

	// Initialize the linked list and determine if overflow is a risk
	large_coord = pelem_array_init(p, n, pnts);

	// Loops over and removes vertices until only two remain
	while (remaining > 3) {
		// If state is unknown, determine it
		if (p[i].state == UNKNOWN) {
			p[i].state = is_reflex(p, i, pnts, large_coord);
		}
		// Only convex vertices with no other points inside the bounding box
		// are valid for removal
		if (p[i].state == CONVEX && !point_in_box(p, i, pnts)) {
			// Remove and output triangle, reset number of fails
			remove_triangle(p, i, pnts, &q);
			remaining--;
			nbr_of_fails = 0;
		}
		else {
			// Vertex state is either reflex or unknown or there are points in
			// the bounding box
			nbr_of_fails++;
		}

		// If true, it means we have looped over all vertices without
		// being able to remove any.
		if (nbr_of_fails >= remaining) {
			// Fast mode failed, start desperate mode
			if (find_removable_vertex(p, &i, pnts, remaining)) {
				// Remove and output triangle, reset number of fails
				remove_triangle(p, i, pnts, &q);
				remaining--;
				nbr_of_fails = 0;
			}
			else {
				// Even desperate mode failed. Triangulation is impossible.
				return 0;
			}
		}

		// Move to next vertex
		i = p[i].prev;
	}

	// Remove last triangle
	remove_triangle(p, i, pnts, &q);
	remaining--;

	if (q - out_start != (n-2)*3 || remaining != 2) {
		// Failure, output should be (n-2)*3 points, and remaining == 2.
		return 0;
	}
	return 1;
}



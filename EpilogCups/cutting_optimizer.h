/*
 * cutting_optimizer.h
 *
 *  Created on: 24.09.2009
 *      Author: amir
 */

#ifndef CUTTING_OPTIMIZER_H_
#define CUTTING_OPTIMIZER_H_

void check_realloc(void *array, int *cnt_elements, int *max_elements, size_t element_size);
bool filter_point(struct point **list, int num_pnt, struct point *pnt);
struct line** iterativeAStar();
int split_intersections(struct line** lines, int *cnt_lines, int *len_lines, struct point** points, int *cnt_points, int* len_points);
bool optimize_vectors(FILE *vector_file, int x_center, int y_center);

#endif /* CUTTING_OPTIMIZER_H_ */

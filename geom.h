/*
 * cutting_optimizer.h
 *
 *  Created on: 22.09.2009
 *      Author: amir
 */

#ifndef CUTTING_OPTIMIZER_H_
#define CUTTING_OPTIMIZER_H_

struct point {
	int x;
	int y;
};

struct polygone {
	float weight;
	struct line *head_line;
	struct line *end_line;
	struct polygone *next;
};


struct line {
	int power;
	struct point *head_point;
	struct point *end_point;
	struct line *next;
};


struct polygone *init_polygone( struct line *line);
void print_polygone( struct polygone *pg );
void print_polygone_list( struct polygone *pg );
void add( struct polygone *new );

struct line *init_line( struct point *head, struct point *end, int power);
void print_line( struct line *line );
void print_line_list( struct line *line );
void add_line( struct polygone* pg, struct line *new );

struct point *init_point( int x, int y);
void print_point( struct point *pnt );
void print_point_list( struct point *pnt );

void check_realloc(void *array, int *cnt_elements, int *max_elements, size_t element_size);
int intersect(struct line *line1, struct line *line2, struct point* intersection);
bool filter_point(struct point **list, int cnt_pnt, struct point *pnt);
extern bool optimize_vectors(FILE *vector_file, int x_center, int y_center);

#endif /* CUTTING_OPTIMIZER_H_ */

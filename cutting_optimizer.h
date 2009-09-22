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
	int power;
	struct point *next;
};

struct polygone {
	float weight;
	struct point *head_point;
	struct point *end_point;
	struct polygone *next;
} ;

struct polygone *init_polygone( struct point *pnt);
void print_polygone( struct polygone *p );
void print_polygone_list( struct polygone *p );
void add( struct polygone *new );

struct point *init_point( int x, int , int power);
void print_point( struct point *pnt );
void print_point_list( struct point *pnt );
void add_point( struct polygone* p, struct point *new );

extern bool optimize_vectors(FILE *vector_file, int x_center, int y_center);

#endif /* CUTTING_OPTIMIZER_H_ */

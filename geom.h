/*
 * cutting_optimizer.h
 *
 *  Created on: 22.09.2009
 *      Author: amir
 */

#ifndef GEOM_H_
#define GEOM_H_


extern const int NOT_INTERSECTING;
extern const int INTERSECTING;
extern const int COINCIDENT;
extern const int PARALLEL;

struct point {
	int x;
	int y;
	struct line **line_refs;
	int num_refs;
	int length_refs;
};

struct polygone {
	float weight;
	struct line *head_line;
	struct line *tail_line;
};


struct line {
	int power;
	struct point *start;
	struct point *end;
	struct line *next;
};


struct polygone *init_polygone( struct line *line);
void print_polygone( struct polygone *pg );

struct point *init_point( int x, int y);
void print_point( struct point *pnt );

struct line *init_line( struct point *head, struct point *end, int power);
void print_line( struct line *line );
void print_line_list( struct line *line );
void add_line( struct polygone* pg, struct line *new );


int intersect(struct line *line1, struct line *line2, struct point* intersection);

#endif /* GEOM_H_ */

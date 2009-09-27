/*
 * cutting_optimizer.c
 *
 *  Created on: 22.09.2009
 *      Author: amir
 */
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "geom.h"

const int NOT_INTERSECTING = -1;
const int INTERSECTING = 0;
const int COINCIDENT = 1;
const int PARALLEL = 2;

struct polygone *init_polygone(struct line *line) {
	struct polygone *pg;
	//wrong size?
	pg = (struct polygone*) calloc(1, sizeof(struct polygone*));
	if (pg == NULL)
		return (struct polygone *) NULL;
	else {
		pg->weight = -1;
		pg->head_line = (struct line *) NULL;
		pg->tail_line = (struct line *) NULL;
		add_line(pg, line);
		return pg;
	}
}


void print_polygone(struct polygone *pg) {
	printf("weigth ->%d\n", pg->weight);
	print_line_list(pg->head_line);
}


/** point functions **/
struct point *init_point(int x, int y) {
	struct point *pnt;
	pnt = (struct point*) calloc(1, sizeof(struct point));
	if (pnt == NULL)
		return (struct point *) NULL;
	else {
		pnt->x = x;
		pnt->y = y;
		pnt->num_refs = 0;
		pnt->length_refs = 50;
		pnt->line_refs = (struct line**) calloc(pnt->length_refs, sizeof(struct line));
		return pnt;
	}
}

void delete_line_ref(struct line *line, struct point *pnt)
{
	int i;
	for(i = 0; i < pnt->length_refs; i++)
	{
		if(pnt->line_refs[i] != NULL && pnt->line_refs[i] == line)
		{
			pnt->line_refs[i] = NULL;
			pnt->num_refs--;
			return;
		}
	}
}

void print_point(struct point *pnt) {
	printf("%d\trefs ->%d\n", pnt, pnt->num_refs);
	printf("%d\tx ->%d\n", pnt, pnt->x);
	printf("%d\ty ->%d\n", pnt, pnt->y);
}

/** line functions **/
struct line *init_line(struct point *head, struct point *end, int power) {
	struct line *line;
	line = (struct line*) calloc(1, sizeof(struct line));
	if (line == NULL)
		return (struct line *) NULL;
	else {
		line->start = head;
		line->end = end;
		line->power = power;

		int init_size = 2;
		check_realloc(head->line_refs, &init_size, &head->length_refs,sizeof(struct line));
		head->line_refs[head->num_refs] = line;
		head->num_refs++;

		check_realloc(end->line_refs, &init_size, &end->length_refs,sizeof(struct line));
		end->line_refs[end->num_refs] = line;
		end->num_refs++;

		return line;
	}
}

void print_line(struct line *line) {
	print_point(line->start);
	print_point(line->end);
}

void print_line_list(struct line *line) {
	while (line != NULL) {
		print_line(line);
		line = line->next;
	}
}

void add_line(struct polygone* pg, struct line *new) {
	if (pg->head_line == NULL)
		pg->head_line = new;
	else
		pg->tail_line->next = new;
	new->next = NULL;
	pg->tail_line = new;
}


 struct point *lineIntersection(struct line* line1, struct line* line2) {

 double distAB, theCos, theSin, newX, ABpos;
 double Ax = line1->head_point->x;
 double Ay = line1->head_point->y;
 double Bx = line1->end_point->x;
 double By = line1->end_point->y;
 double Cx = line2->head_point->x;
 double Cy = line2->head_point->x;
 double Dx = line2->end_point->x;
 double Dy = line2->end_point->x;

 //  Fail if either line is undefined.
 if (Ax == Bx && Ay == By || Cx == Dx && Cy == Dy)
 return (struct point *) NULL;

 //  (1) Translate the system so that point A is on the origin.
 Bx -= Ax;
 By -= Ay;
 Cx -= Ax;
 Cy -= Ay;
 Dx -= Ax;
 Dy -= Ay;

 //  Discover the length of segment A-B.
 distAB = sqrt(Bx * Bx + By * By);

 //  (2) Rotate the system so that point B is on the positive X axis.
 theCos = Bx / distAB;
 theSin = By / distAB;
 newX = Cx * theCos + Cy * theSin;
 Cy = Cy * theCos - Cx * theSin;
 Cx = newX;
 newX = Dx * theCos + Dy * theSin;
 Dy = Dy * theCos - Dx * theSin;
 Dx = newX;

 //  Fail if the lines are parallel.
 if (Cy == Dy)
 return PARALLEL;

 //  (3) Discover the position of the intersection point along line A-B.
 ABpos = Dx + (Cx - Dx) * Dy / (Dy - Cy);

 //  (4) Apply the discovered position to line A-B in the original coordinate system.
 return init_point(Ax + ABpos * theCos, Ay + ABpos * theSin);
 }

int intersect2(struct line *line1, struct line *line2,
		struct point* intersection) {
       float denom = ((line2->end->y - line2->start->y)*(line1->end->x - line1->start->x)) -
                     ((line2->end->x - line2->start->x)*(line1->end->y - line1->start->y));

       float nume_a = ((line2->end->x - line2->start->x)*(line1->start->y - line2->start->y)) -
                      ((line2->end->y - line2->start->y)*(line1->start->x - line2->start->x));

       float nume_b = ((line1->end->x - line1->start->x)*(line1->start->y - line2->start->y)) -
                      ((line1->end->y - line1->start->y)*(line1->start->x - line2->start->x));

       if(denom == 0.0f)
       {
           if(nume_a == 0.0f && nume_b == 0.0f)
           {
               return COINCIDENT;
           }
           return PARALLEL;
       }

       float ua = nume_a / denom;
       float ub = nume_b / denom;

       if(ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f)
       {
           // Get the intersection point.
           intersection->x = line1->start->x + ua*(line1->end->x - line1->start->x);
           intersection->y = line1->start->y + ua*(line1->end->y - line1->start->y);

           return INTERSECTING;
       }

       return NOT_INTERSECTING;
   }


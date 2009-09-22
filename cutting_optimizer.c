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
#include "cutting_optimizer.h"

struct polygone *head_polygone = (struct polygone *) NULL;
struct polygone *end_polygone = (struct polygone *) NULL;
int pg_cnt = 0;

struct polygone *init_polygone(struct point *pnt) {
	struct polygone *pg;
	pg = (struct polygone*) calloc(1, sizeof(struct polygone));
	if (pg == NULL)
		return (struct polygone *) NULL;
	else {
		pg->weight = -1;
		pg->head_point = (struct point *) NULL;
		pg->end_point = (struct point *) NULL;
		return pg;
	}
}

void print_point_list(struct point *pnt) {
	while (pnt != NULL) {
		print_point(pnt);
		pnt = pnt->next;
	}
}

void print_polygone(struct polygone *pg) {
	printf("weigth ->%d\n", pg->weight);
	print_point_list(pg->head_point);
}

void print_polygone_list(struct polygone *pg) {
	while (pg != NULL) {
		print_polygone(pg);
		pg = pg->next;
	}
}

void add_polygone(struct polygone *new) {
	if (head_polygone == NULL) {
		head_polygone = new;
	} else {
		end_polygone->next = new;
	}
	new->next = NULL;
	end_polygone = new;
	pg_cnt++;
}

struct point *init_point(int x, int y, int power) {
	struct point *pnt;
	pnt = (struct point*) calloc(1, sizeof(struct point));
	if (pnt == NULL)
		return (struct point *) NULL;
	else {
		pnt->x = x;
		pnt->y = y;
		pnt->power = power;
		return pnt;
	}
}

void print_point(struct point *pnt) {
	printf("x ->%d\n", pnt->x);
	printf("y ->%d\n", pnt->y);
}

void add_point(struct polygone* pg, struct point *new) {
	if (pg->head_point == NULL)
		pg->head_point = new;
	else
		pg->end_point->next = new;
	new->next = NULL;
	pg->end_point = new;
}

void calculate_weight(struct polygone* pg, int x_center, int y_center) {
	struct point *pnt = pg->head_point;
	pg->weight = 0;
	int diff;
	while (pnt != NULL) {
		diff = abs(x_center - pnt->x) + abs(y_center - pnt->y);
		if (diff > pg->weight)
			pg->weight = diff;

		pnt = pnt->next;
	}
}

struct polygone *bubbleSort(struct polygone* head, int count) {
	int i, j;
	struct polygone *p0, *p1, *p2, *p3;
	for (i = 1; i < count; i++) {
		p0 = (struct polygone *) NULL;
		p1 = head;
		p2 = head->next;
		p3 = p2->next;
		for (j = 1; j <= (count - i); j++) {
			if (p1->weight > p2->weight) {
				// Adjust the pointers...
				p1->next = p3;
				p2->next = p1;
				if (p0)
					p0->next = p2;
				// Set the head pointer if it was changed...
				if (head == p1)
					head = p2;
				// Progress the pointers
				p0 = p2;
				p2 = p1->next;
				if (p3 != NULL)
					p3 = p3->next != (struct polygone *) NULL ? p3->next
							: (struct polygone *) NULL;
			} else {
				// Nothing to swap, just progress the pointers...
				p0 = p1;
				p1 = p2;
				p2 = p3;
				p3 = p3->next != (struct polygone *) NULL ? p3->next
						: (struct polygone *) NULL;
			}
		}
	}
	return (head);
}

bool optimize_vectors(FILE *vector_file, int x_center, int y_center) {
	char buf[102400];
	int lx;
	int ly;
	int offx;
	int offy;
	int power;
	int mx;
	int my;
	struct polygone *pg = (struct polygone *) NULL;

	while (fgets((char *) buf, sizeof(buf), vector_file)) {
		if (isalpha(*buf)) {
			int x, y;
			switch (*buf) {
			case 'M': // move
				if (sscanf((char *) buf + 1, "%d,%d", &y, &x) == 2) {
					mx = x;
					my = y;
					if (mx != lx || my != ly) {
						if (pg != NULL)
							calculate_weight(pg, x_center, y_center);
						pg = init_polygone(init_point(mx, lx, power));
						add_polygone(pg);
					}
				}
				break;
			case 'C': // close - only makes sense after an "L"
				break;
			case 'P': // power
				if (sscanf((char *) buf + 1, "%d", &x) == 1) {
					power = x;
				}
				break;
			case 'L': // line
				if (sscanf((char *) buf + 1, "%d,%d", &y, &x) == 2) {
					lx = x;
					ly = y;
					add_point(pg, init_point(x, y, power));
				}
				break;
			}

			if (*buf == 'X')
				break;
		}
	}

	head_polygone = bubbleSort(head_polygone, pg_cnt);
	print_polygone_list(head_polygone);
	return true;
}


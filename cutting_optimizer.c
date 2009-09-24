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
#include "geom.h"
#include <math.h>

struct polygone *head_polygone = (struct polygone *) NULL;
struct polygone *end_polygone = (struct polygone *) NULL;
int pg_cnt = 0;
const int NOT_INTERSECTING = -1;
const int INTERSECTING = 0;
const int COINCIDENT = 1;
const int PARALLEL = 2;

struct polygone *init_polygone(struct line *line) {
	struct polygone *pg;
	pg = (struct polygone*) calloc(1, sizeof(struct polygone*));
	if (pg == NULL)
		return (struct polygone *) NULL;
	else {
		pg->weight = -1;
		pg->head_line = (struct line *) NULL;
		pg->end_line = (struct line *) NULL;
		add_line(pg, line);
		return pg;
	}
}

void print_polygone(struct polygone *pg) {
	printf("weigth ->%d\n", pg->weight);
	print_line_list(pg->head_line);
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

/** point functions **/
struct point *init_point(int x, int y) {
	struct point *pnt;
	pnt = (struct point*) calloc(1, sizeof(struct point));
	if (pnt == NULL)
		return (struct point *) NULL;
	else {
		pnt->x = x;
		pnt->y = y;
		return pnt;
	}
}

void print_point(struct point *pnt) {
	printf("x ->%d\n", pnt->x);
	printf("y ->%d\n", pnt->y);
}

/** line functions **/
struct line *init_line(struct point *head, struct point *end, int power) {
	struct line *line;
	line = (struct line*) calloc(1, sizeof(struct line));
	if (line == NULL)
		return (struct line *) NULL;
	else {
		line->head_point = head;
		line->end_point = end;
		line->power = power;
		return line;
	}
}

void print_line(struct line *line) {
	print_point(line->head_point);
	print_point(line->end_point);
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
		pg->end_line->next = new;
	new->next = NULL;
	pg->end_line = new;
}

/*void calculate_weight(struct polygone* pg, int x_center, int y_center) {
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
 }*/

/** brute force by now **/
bool filter_point(struct point **list, int num_pnt, struct point *pnt) {
	int i;

	for (i = 0; i < num_pnt; i++) {
		if (list[i]->x == pnt->x && list[i]->y == pnt->y) {
			pnt = list[i];
			return true;
		}
	}
	return false;
}

int intersect(struct line *line1, struct line *line2,
		struct point* intersection) {
	float denom = ((line2->end_point->y - line2->head_point->y)
			* (line1->end_point->x - line1->head_point->x))
			- ((line2->end_point->x - line2->head_point->x)
					* (line1->end_point->y - line1->head_point->y));

	float nume_a = ((line2->end_point->x - line2->head_point->x)
			* (line1->head_point->y - line2->head_point->y))
			- ((line2->end_point->y - line2->head_point->y)
					* (line1->head_point->x - line2->head_point->x));

	float nume_b = ((line1->end_point->x - line1->head_point->x)
			* (line1->head_point->y - line2->head_point->y))
			- ((line1->end_point->y - line1->head_point->y)
					* (line1->head_point->x - line2->head_point->x));

	if (denom == 0.0f) {
		if (nume_a == 0.0f && nume_b == 0.0f) {
			return COINCIDENT;
		}
		return PARALLEL;
	}

	float ua = nume_a / denom;
	float ub = nume_b / denom;

	if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f) {
		// Get the intersection point.
		int x = line1->head_point->x + ua * (line1->end_point->x
				- line1->head_point->x);
		int y = line1->head_point->y + ua * (line1->end_point->y
				- line1->head_point->y);
		intersection = init_point(x, y);
		return INTERSECTING;
	}

	return NOT_INTERSECTING;
}

int split_intersections(struct line** lines, int *cnt_lines, int *len_lines) {

	int i;
	int j;
	struct point *intersec;
	struct line *l1, *l2;
	struct line *split1, *split2, *split3, *split4;
	for (i = 0; i < *cnt_lines; i++) {
		l1 = lines[i];

		if (l1 == NULL)
			continue;

		for (j = 0; j < *cnt_lines; j++) {
			if (i != j) {
				l2 = lines[j];
				if (l2 == NULL)
					continue;
				printf("%d %d\n", i,j);
				if (intersect(l1, l2, intersec) == INTERSECTING) {
					lines[i] = (struct line *) NULL;
					lines[j] = (struct line *) NULL;

					if ((l1 ->head_point->x != intersec->x || l1 ->head_point->y
							!= intersec->y) && (l1 ->end_point->x != intersec->x || l1 ->end_point->y
									!= intersec->y)) {

						check_realloc(lines, cnt_lines + 2, len_lines, sizeof(struct line));

						split1 = init_line(l1->head_point, intersec, l1->power);
						lines[*cnt_lines] = split1;
						(*cnt_lines)++;

						split2 = init_line(intersec, l1->end_point, l1->power);
						lines[*cnt_lines] = split2;
						(*cnt_lines)++;
					}
					if ((l2 ->head_point->x != intersec->x || l2 ->head_point->y
							!= intersec->y) && (l2 ->end_point->x != intersec->x || l2 ->end_point->y
									!= intersec->y)) {
						check_realloc(lines, cnt_lines + 2, len_lines, sizeof(struct line));

						split3 = init_line(l2->head_point, intersec, l2->power);
						lines[*cnt_lines] = split3;
						(*cnt_lines)++;

						split4 = init_line(intersec, l2->end_point, l2->power);
						lines[*cnt_lines] = split4;
						(*cnt_lines)++;
					}
					//printf("%d %d\n", *cnt_lines, *len_lines);

				}

			}
		}
	}

}
/*
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
 return (struct point *) NULL;

 //  (3) Discover the position of the intersection point along line A-B.
 ABpos = Dx + (Cx - Dx) * Dy / (Dy - Cy);

 //  (4) Apply the discovered position to line A-B in the original coordinate system.
 return init_point(Ax + ABpos * theCos, Ay + ABpos * theSin);
 }
 */
void check_realloc(void *array, int *cnt_elements, int *max_elements, size_t element_size) {
	if (*cnt_elements >= *max_elements) {
		*max_elements *= 2;
		array = realloc(array, *max_elements * element_size);
	}
}

bool optimize_vectors(FILE *vector_file, int x_center, int y_center) {
	char buf[102400];
	int power;
	int prealloc = 1000;
	struct point *m_pnt;
	struct point **points = calloc(prealloc, sizeof(struct point));
	struct line **lines = calloc(prealloc, sizeof(struct line ));
	int num_points = prealloc;
	int num_lines = prealloc;
	int cnt_points = 0;
	int cnt_lines = 0;
	while (fgets((char *) buf, sizeof(buf), vector_file)) {
		if (isalpha(*buf)) {
			int x, y;
			struct point *pnt;

			switch (*buf) {
			case 'M': // move
				if (sscanf((char *) buf + 1, "%d,%d", &y, &x) == 2) {
					m_pnt = init_point(x, y);

					check_realloc(points, &cnt_points, &num_points, sizeof(struct point));

					if (!filter_point(points, cnt_points, m_pnt)) {
						points[cnt_points] = m_pnt;
						cnt_points++;
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
					pnt = init_point(x, y);

					check_realloc(lines, &cnt_lines, &num_lines, sizeof(struct line));

					if (!filter_point(points, cnt_points, pnt)) {
						points[cnt_points] = pnt;
						cnt_points++;
					}
					lines[cnt_lines] = init_line(m_pnt, pnt, power);
					cnt_lines++;
				}
				break;
			}

			if (*buf == 'X')
				break;
		}
	}
	int i = 0;

	for (i = 0; i < cnt_lines; i++) {
		print_line(lines[i]);
	}
	split_intersections(lines, &cnt_lines, &num_lines);
	printf("split\n");
	for (i = 0; i < cnt_lines; i++) {
		if (lines[i] != NULL)
			print_line(lines[i]);
	}

	//head_polygone = bubbleSort(head_polygone, pg_cnt);
	//print_polygone_list(head_polygone);
	return true;
}


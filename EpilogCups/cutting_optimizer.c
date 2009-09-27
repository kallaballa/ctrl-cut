/*
 * cutting_optimizer.c
 *
 *  Created on: 22.09.2009
 *      Author: amir
 */


#include <netdb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "geom.h"
#include "cutting_optimizer.h"

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


struct line** iterativeAStar()
{
//	int i, j, b, b_new;
	return NULL;

}

int split_intersections(struct line** lines, int *cnt_lines, int *len_lines, struct point** points, int *cnt_points, int* len_points) {
	int i;
	int j;
	struct point *intersec = init_point(-1,-1);
	struct line *l1, *l2;
	struct line *split1, *split2, *split3, *split4;
	bool added;
	for (i = 0; i < *cnt_lines; i++) {
		l1 = lines[i];

		if (l1 == NULL)
			continue;

		for (j = 0; j < *cnt_lines; j++) {
			added = false;
			if (i != j) {
				l2 = lines[j];
				if (l2 == NULL)
					continue;
				printf("%d %d\n", i,j);
				if (intersect(l1, l2, intersec) == INTERSECTING) {
					if ((l1 ->start->x != intersec->x  || l1 ->start->y
							!= intersec->y) && (l1 ->end->x != intersec->x || l1 ->end->y
									!= intersec->y)) {

						int ralloc_l = (*cnt_lines) + 2;
						int ralloc_p = (*cnt_points) + 1;

						if (!filter_point(points, *cnt_points, intersec)) {
							check_realloc(points, &ralloc_p , len_points, sizeof(struct point));
							points[*cnt_points] = intersec;
							(*cnt_points)++;
							added = true;
						}


						check_realloc(lines, &ralloc_l , len_lines, sizeof(struct line));

						split1 = init_line(l1->start, intersec, l1->power);
						lines[*cnt_lines] = split1;
						(*cnt_lines)++;

						split2 = init_line(intersec, l1->end, l1->power);
						lines[*cnt_lines] = split2;
						(*cnt_lines)++;

						delete_line_ref(lines[i], l1->start);
						delete_line_ref(lines[i], l1->end);
						lines[i] = (struct line *) NULL;
						lines[j] = (struct line *) NULL;
					}
					if ((l2 ->start->x != intersec->x || l2 ->start->y
							!= intersec->y) && (l2 ->end->x != intersec->x || l2 ->end->y
									!= intersec->y)) {
						int ralloc = (*cnt_lines) + 2;
						int ralloc_p = (*cnt_points) + 1;


						if (!added && !filter_point(points, *cnt_points, intersec)) {
							check_realloc(points, &ralloc_p , len_points, sizeof(struct point));
							points[*cnt_points] = intersec;
							(*cnt_points)++;
						}

						check_realloc(lines, &ralloc, len_lines, sizeof(struct line));
						split3 = init_line(l2->start, intersec, l2->power);
						lines[*cnt_lines] = split3;
						(*cnt_lines)++;

						split4 = init_line(intersec, l2->end, l2->power);
						lines[*cnt_lines] = split4;
						(*cnt_lines)++;

						delete_line_ref(lines[j], l2->start);
						delete_line_ref(lines[j], l2->end);
						lines[i] = (struct line *) NULL;
						lines[j] = (struct line *) NULL;
					}
					//printf("%d %d\n", *cnt_lines, *len_lines);

				}

			}
		}
	}
}

void check_realloc(void *array, int *cnt_elements, int *length, size_t element_size) {
	if (*cnt_elements >= *length) {
		*length *= 2;
		array = realloc(array, *length * element_size);
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


	split_intersections(lines, &cnt_lines, &num_lines, points, &cnt_points, &num_points);
	for (i = 0; i < cnt_points; i++) {
		print_point(points[i]);
	}
	printf("split\n");
	for (i = 0; i <= cnt_lines; i++) {
		if (lines[i] != NULL)
			print_line(lines[i]);
	}
	printf("%d\n", cnt_points);
	printf("%d\n", cnt_lines);
	//head_polygone = bubbleSort(head_polygone, pg_cnt);
	//print_polygone_list(head_polygone);
	return true;
}



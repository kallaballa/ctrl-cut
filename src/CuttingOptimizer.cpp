/*
 * CuttingOptimizer.cpp
 *
 *  Created on: 26.09.2009
 *      Author: amir
 */
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>
#include "LSPoint.h"
#include "LineSegment.h"
#include "CuttingOptimizer.h"


using namespace std;

vector<LSPoint*> points;
vector<LineSegment*> lines;

CuttingOptimizer::CuttingOptimizer() {
	// TODO Auto-generated constructor stub

}

CuttingOptimizer::~CuttingOptimizer() {
	// TODO Auto-generated destructor stub
}

void print_point(Point *pnt) {
	printf("%d\tx ->%d\n", pnt, pnt->x);
	printf("%d\ty ->%d\n", pnt, pnt->y);
}

void print_line(LineSegment *ls) {
	if (ls == NULL)
		return;
	print_point(ls->getStart());
	print_point(ls->getEnd());
}

bool addPoint(LSPoint* p) {
	unsigned int i;

	LSPoint* p_stored;
	for (i = 0; i < points.size(); i++) {
		p_stored = points.at(i);
		if (p_stored->equals(p)) {
			p = p_stored;
			return false;
		}
	}

	points.push_back(p);
	return true;
}

void addLine(LSPoint *start, LSPoint *end, int power) {
	LineSegment *ls = new LineSegment(start, end, power);
	if (addPoint(start)) {
		start->addParent(ls);
		ls->setStart(start);
	}

	if (addPoint(end)) {
		end->addParent(ls);
		ls->setEnd(end);
	}

	lines.push_back(ls);
}

void splitAtIntersections() {
	unsigned int i, j;
	LSPoint *intersec = new LSPoint(-1, -1);
	LineSegment *split1, *split2, *split3, *split4;
	LineSegment *ls1, *ls2;

	for (i = 0; i < lines.size(); i++) {
		ls1 = lines.at(i);

		if (ls1 == NULL)
			continue;

		for (j = 0; j < lines.size(); j++) {
			ls2 = lines.at(j);

			if (ls2 == NULL || ls1 == ls2)
				continue;

			if (ls1->intersects(ls2, intersec) == LineSegment::INTERSECTING) {
				addPoint(intersec);
				if (!ls1->getStart()->equals(intersec)
						&& !ls1->getEnd()->equals(intersec)) {
					split1 = new LineSegment(ls1->getStart(), intersec,
							ls1->getPower());
					split2 = new LineSegment(ls1->getEnd(), intersec,
							ls1->getPower());
					replace(lines.begin(), lines.end(), ls1,
							(LineSegment*) NULL);

					lines.push_back(split1);
					lines.push_back(split2);
				}

				if (!ls2->getStart()->equals(intersec)
						&& !ls2->getEnd()->equals(intersec)) {
					split3 = new LineSegment(ls2->getStart(), intersec,
							ls2->getPower());
					split4 = new LineSegment(ls2->getEnd(), intersec,
							ls2->getPower());
					replace(lines.begin(), lines.end(), ls2,
							(LineSegment*) NULL);
					lines.push_back(split3);
					lines.push_back(split4);
				}
			}
		}
	}
}

void optimize_vectors(char *vector_file, int x_center, int y_center) {
	string line;
	ifstream infile(vector_file, ios_base::in);
	char first;
	int power, x, y;
	int lx, ly;
	int mx, my;
	LSPoint *start;
	LSPoint *end;
	LineSegment *ls;

	while (getline(infile, line, '\n')) {
		first = *line.begin();

		if (first == 'X')
			break;

		if (isalpha(first)) {
			switch (first) {
			case 'M': // move
				if (sscanf((char *) line.data() + 1, "%d,%d", &y, &x) == 2) {
					lx = x;
					ly = y;
					mx = x;
					my = y;
				}
				break;
			case 'C': // close
				if (lx != mx || ly != my) {
					addLine(new LSPoint(lx, ly), new LSPoint(mx, my), power);
				}
				break;
			case 'P': // power
				if (sscanf((char *) line.data() + 1, "%d", &x) == 1) {
					power = x;
				}
				break;
			case 'L': // line
				if (sscanf((char *) line.data() + 1, "%d,%d", &y, &x) == 2) {
					start = new LSPoint(lx, ly);
					end = new LSPoint(x, y);
					addLine(start, end, power);
					lx = x;
					ly = y;
				}
				break;
			}
		}
	}
	infile.close();
//	splitAtIntersections();

	ofstream outfile;
	 outfile.open ("/tmp/out.vector", ofstream::out | ofstream::trunc);

	unsigned int i;

	printf("%d\n", lines.size());

	for (i = 0; i < lines.size(); i++) {
		ls = lines.at(i);
		if (ls != NULL) {
			print_line(ls);
			outfile << "P";
			 outfile << ls->getPower();
			 outfile << "\n";
			 outfile << "M";
			 outfile << ls->getStart()->x;
			 outfile << ",";
			 outfile << ls->getStart()->y;
			 outfile << "\n";
			 outfile << "L";
			 outfile << ls->getStart()->x;
			 outfile << ",";
			 outfile << ls->getStart()->y;
			 outfile << "\n";
			 outfile << "L";
			 outfile << ls->getEnd()->x;
			 outfile << ",";
			 outfile << ls->getEnd()->y;
			 outfile << "\n";
			 outfile << "M";
			 outfile << ls->getEnd()->x;
			 outfile << ",";
			 outfile << ls->getEnd()->y;
			 outfile << "\n";
		}

	}
	outfile << "X\n";
	outfile.close();

}

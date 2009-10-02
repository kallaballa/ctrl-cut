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
#include <list>
#include <stack>
#include <set>
#include <map>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <cmath>
#include "LineSegment.h"
#include "Polygon.h"
#include "CuttingOptimizer.h"

using namespace std;

map<string, LineSegment::LSPoint*> points;
list<LineSegment*> lines;

CuttingOptimizer::CuttingOptimizer() {
	// TODO Auto-generated constructor stub
}

CuttingOptimizer::~CuttingOptimizer() {
	// TODO Auto-generated destructor stub
}

void print_point(LineSegment::LSPoint *pnt) {
	printf("x ->%d\ty ->%d\t%d\t%s\t%d\n", pnt->getX(), pnt->getY(),
			pnt->getConnectors().size(), pnt->getKey().data(), pnt);
}

void print_line(LineSegment *ls) {
	if (ls == NULL)
		return;
	printf("Line:%d\n", ls);
	print_point((LineSegment::LSPoint*) ls->getStart());
	print_point((LineSegment::LSPoint*) ls->getEnd());
	printf("\n");
}

LineSegment::LSPoint* addPoint(LineSegment::LSPoint* p) {
	map<string, LineSegment::LSPoint*>::iterator it = points.find(p->getKey());

	if (it != points.end()) {
		return (LineSegment::LSPoint*) it->second;
	}

	points.insert(pair<string, LineSegment::LSPoint*> (p->getKey(), p));
	return p;
}

void addLine(LineSegment::LSPoint *start, LineSegment::LSPoint *end, int power) {
	LineSegment *ls = new LineSegment(start, end, power);

	start = addPoint(start);
	ls->setStart(start);
	start->addConnector(ls);

	end = addPoint(end);
	ls->setEnd(end);
	end->addConnector(ls);

	lines.push_back(ls);
}

list<LineSegment*>::iterator eraseLine(list<LineSegment*>::iterator it_ls) {
	LineSegment *ls = *it_ls;
	ls->getStart()->removeConnector(ls);
	ls->getEnd()->removeConnector(ls);

	return lines.erase(it_ls);
}

void splitAtIntersections() {
	LineSegment::LSPoint *intersec = new LineSegment::LSPoint(-1000, -1000);
	LineSegment *ls1, *ls2;

	list<LineSegment*>::iterator it_i;
	list<LineSegment*>::iterator it_j;

	for (it_i = lines.begin(); it_i != lines.end(); it_i++) {
		for (it_j = lines.begin(); it_j != lines.end(); it_j++) {
			ls2 = *it_j;
			ls1 = *it_i;

			if(it_i == lines.end())
				break;

			if (ls1 == ls2)
				continue;

			if ((intersec = ls1->intersects(ls2)) != NULL) {
				intersec = addPoint(intersec);

				if (!ls1->getStart()->equals(intersec)
						&& !ls1->getEnd()->equals(intersec)) {
					it_i = eraseLine(it_i);
					addLine((LineSegment::LSPoint*) ls1->getStart(), intersec,
							ls1->getPower());
					addLine((LineSegment::LSPoint*) ls1->getEnd(), intersec,
							ls1->getPower());
				}

				if (!ls2->getStart()->equals(intersec)
						&& !ls2->getEnd()->equals(intersec)) {
					it_j = eraseLine(it_j);
					addLine((LineSegment::LSPoint*) ls2->getStart(), intersec,
							ls2->getPower());
					addLine((LineSegment::LSPoint*) ls2->getEnd(), intersec,
							ls2->getPower());
				}
			}
		}
	}
}

/*LineSegment* find_next(vector<LineSegment*> occupied, LineSegment* current) {
 vector<LineSegment*> connectors = ((LSPoint*)current->getEnd())->getConnectors();

 LineSegment* candidate;
 LineSegment* best = NULL;

 float current_slope = current->getSlope();
 float best_slope_diff = 0.0f;
 float slop_diff;

 unsigned int i;
 for (i = 0; i < connectors.size(); i++) {
 candidate = connectors.at(i);

 if (!find_line(occupied, candidate)) {
 if(candidate->getStart() != current->getEnd())
 {
 candidate->setEnd(candidate->getStart());
 candidate->setStart(current->getEnd());
 }

 slop_diff = fabs(current_slope - candidate->getSlope());
 if (slop_diff > best_slope_diff){
 best = candidate;
 best_slope_diff = slop_diff;
 }
 }
 }

 return best;
 }*/

void find_connected(set<LineSegment*> *occupied, Polygon *polygon,
		LineSegment* current) {
	set<LineSegment*> connectors = current->getEnd()->getConnectors();
	set<LineSegment*>::iterator it;
	LineSegment* candidate;

	occupied->insert(current);

	for (it = connectors.begin(); it != connectors.end(); it++) {
		candidate = *it;
		if (candidate == current || occupied->find(candidate) != occupied->end())
			continue;

		if (candidate->getStart() != current->getEnd()) {
			candidate->setEnd(candidate->getStart());
			candidate->setStart(current->getEnd());
		}
		polygon->addLineSegment(candidate);
		find_connected(occupied, polygon, candidate);
	}
}

vector<Polygon*> find_polygons() {

	vector<Polygon*> polygons;
	set<LineSegment*> *occupied = new set<LineSegment*>();

	LineSegment* ls;
	list<LineSegment*>::iterator it;

	for (it = lines.begin(); it != lines.end(); it++) {
		ls = *it;

		Polygon *polygon = new Polygon();

		if (occupied->find(ls) == occupied->end()) {
			polygon->addLineSegment(ls);
			find_connected(occupied, polygon, ls);
			polygons.push_back(polygon);
		}
	}
	return polygons;
}

void optimize_vectors(char *vector_file, int x_center, int y_center) {
	string line;
	ifstream infile(vector_file, ios_base::in);
	char first;
	int power, x, y;
	int lx, ly;
	int mx, my;
	LineSegment::LSPoint *start;
	LineSegment::LSPoint *end;
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
					addLine(new LineSegment::LSPoint(lx, ly),
							new LineSegment::LSPoint(mx, my), power);
				}
				break;
			case 'P': // power
				if (sscanf((char *) line.data() + 1, "%d", &x) == 1) {
					power = x;
				}
				break;
			case 'L': // line
				if (sscanf((char *) line.data() + 1, "%d,%d", &y, &x) == 2) {
					start = new LineSegment::LSPoint(lx, ly);
					end = new LineSegment::LSPoint(x, y);
					addLine(start, end, power);
					lx = x;
					ly = y;
				}
				break;
			}
		}
	}
	infile.close();


	printf("points: %d\n", points.size());

	map<string, LineSegment::LSPoint*>::iterator it_p;

	for (it_p = points.begin(); it_p != points.end(); it_p++) {
		print_point(it_p->second);
	}

	printf("lines: %d\n", lines.size());

	list<LineSegment*>::iterator it;

	for (it = lines.begin(); it != lines.end(); it++) {
			print_line(*it);
	}

	splitAtIntersections();
	unsigned int i, j;

	vector<Polygon*> polygones = find_polygons();
	vector<LineSegment*> segments;

	for (i = 0; i < polygones.size(); i++) {
		segments = polygones.at(i)->getLineSegments();
		printf("Polygon: %d\n", polygones.size());

		for (j = 0; j < segments.size(); j++) {
			print_line(segments.at(j));
		}
		printf("\n");
	}

	return;

	ofstream outfile;
	outfile.open(vector_file, ofstream::out | ofstream::trunc);
	printf("%d\n", lines.size());

	list<LineSegment*>::iterator it_i;

	for (it_i = lines.begin(); it_i != lines.end(); it_i++) {
		ls = *it_i;
		if (ls != NULL) {
			print_line(ls);
			outfile << "P";
			outfile << ls->getPower();
			outfile << "\n";
			outfile << "M";
			outfile << ls->getStart()->getX();
			outfile << ",";
			outfile << ls->getStart()->getY();
			outfile << "\n";
			outfile << "L";
			outfile << ls->getStart()->getX();
			outfile << ",";
			outfile << ls->getStart()->getY();
			outfile << "\n";
			outfile << "L";
			outfile << ls->getEnd()->getX();
			outfile << ",";
			outfile << ls->getEnd()->getY();
			outfile << "\n";
			/*			 outfile << "M";
			 outfile << ls->getEnd()->getX();
			 outfile << ",";
			 outfile << ls->getEnd()->getY();
			 outfile << "\n";*/
		}

	}
	outfile << "X\n";
	outfile.close();

}

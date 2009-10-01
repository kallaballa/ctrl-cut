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
#include <set>
#include <fstream>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <cmath>
#include "LSPoint.h"
#include "LineSegment.h"
#include "Polygon.h"
#include "CuttingOptimizer.h"

using namespace std;

set<LSPoint*> points;
set<LineSegment*> lines;

CuttingOptimizer::CuttingOptimizer() {
	// TODO Auto-generated constructor stub

}

CuttingOptimizer::~CuttingOptimizer() {
	// TODO Auto-generated destructor stub
}

void print_point(LSPoint *pnt) {
	printf("x ->%d\ty ->%d\t%d\n", pnt->getX(), pnt->getY(), pnt->getConnectors().size());
}

void print_line(LineSegment *ls) {
	if (ls == NULL)
		return;
	print_point((LSPoint*)ls->getStart());
	print_point((LSPoint*)ls->getEnd());
}

bool addPoint(LSPoint* p) {
	pair<set<LSPoint*>::iterator,bool> ret;
	ret = points.insert(p);
	if(!ret.second)
	{
		p = *(ret.first);
		return false;
	}

	return true;
}

void addLine(LSPoint *start, LSPoint *end, int power) {
	LineSegment *ls = new LineSegment(start, end, power);

	pair<set<LineSegment*>::iterator,bool> ret;
	ret = lines.insert(ls);

	addPoint(start);
	ls->setStart(start);
	start->addConnector(ls);

	addPoint(end);
	ls->setEnd(end);
	end->addConnector(ls);
}

void splitAtIntersections() {
	LSPoint *intersec = new LSPoint(-1, -1);
	LineSegment *ls1, *ls2;
	set<LineSegment*>::iterator it_i, it_j;
	printf("lines: %d\n", lines.size());
	printf("points: %d\n", lines.size());

	for (it_i=lines.begin(); it_i!=lines.end(); it_i++) {
		ls1 = *it_i;

		for (it_j=lines.begin(); it_j!=lines.end(); it_j++) {
			ls2 = *it_j;

			if (ls1 == ls2)
				continue;

			if (ls1->intersects(ls2, intersec) == LineSegment::INTERSECTING) {
				addPoint(intersec);
				if (!ls1->getStart()->equals(intersec)
						&& !ls1->getEnd()->equals(intersec)) {
					addLine((LSPoint*)ls1->getStart(), intersec, ls1->getPower());
					addLine((LSPoint*)ls1->getEnd(), intersec, ls1->getPower());
					lines.erase(ls1);
				}

				if (!ls2->getStart()->equals(intersec)
						&& !ls2->getEnd()->equals(intersec)) {
					addLine((LSPoint*)ls2->getStart(), intersec, ls2->getPower());
					addLine((LSPoint*)ls2->getEnd(), intersec, ls2->getPower());
					lines.erase(ls2);
				}
			}
		    it_j = lines.find(ls2);
		}
		it_i = lines.find(ls1);
	}
}


bool find_line(vector<LineSegment*> *list, LineSegment* ls) {
	unsigned int i;
	for (i = 0; i < list->size(); i++) {
		if (list->at(i) == ls)
			return true;
	}
	return false;
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
/*
void find_connected(vector<LineSegment*>* occupied, Polygon* polygon, LineSegment* current) {
	vector<LineSegment*> connectors = ((LSPoint*)current->getEnd())->getConnectors();
	LineSegment* candidate;
	unsigned i;

	for(i = 0; i < connectors.size(); i++)
	{
		candidate = connectors.at(i);
		if(candidate == current)
			continue;
		if(candidate->getStart() != current->getEnd())
		{
			candidate->setEnd(candidate->getStart());
			candidate->setStart(current->getEnd());	set<LSPoint*>::iterator it_p;

	 for (it_p=points.begin(); it_i!=lines.end(); it_i++) {

	 }

		}

		if(!find_line(occupied, candidate))
		{
			occupied->push_back(candidate);
			polygon->addLineSegment(candidate);
			find_connected(occupied, polygon, candidate);
		}
	}
}*/

/*vector<Polygon*> find_polygons()
{

	vector<Polygon*> polygons;

	LineSegment* ls;
	unsigned int i;
	for(i = 0; i < lines.size(); i++)
	{
		ls = lines.at(i);
		if(ls == NULL)
			continue;

		Polygon *polygon = new Polygon();

		if(!find_line(occupied,ls))
		{
			find_connected(occupied, polygon,ls);
			polygons.push_back(polygon);
		}
	}
	return polygons;
}*/

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
	set<LSPoint*>::iterator it_p;

	 for (it_p=points.begin(); it_p!=points.end(); it_p++) {
		 print_point(*it_p);
	 }

	splitAtIntersections();
	unsigned int i,j;

	/*	vector<Polygon*> polygones = find_polygons();
	vector<LineSegment*> segments;

	for (i = 0; i < polygones.size(); i++) {
		segments = polygones.at(i)->getLineSegments();
		printf("Polygon: %d\n", polygones.size());

		for (j = 0; j < segments.size(); j++) {
			print_line(segments.at(j));
		}
		printf("\n");
	}*/


	ofstream outfile;
	outfile.open(vector_file, ofstream::out | ofstream::trunc);
	printf("%d\n", lines.size());

	for (i = 0; i < lines.size(); i++) {
		//ls = lines.at(i);
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

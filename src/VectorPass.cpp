/*
 * VectorCut.cpp
 *
 *  Created on: 04.10.2009
 *      Author: amir
 */

#include "VectorPass.h"
#include <fstream>
#include <iostream>


map<string, Joint*> VectorPass::getJoints() {
	return this->joints;
}

list<LineSegment*> VectorPass::getLineSegements() {
	return this->lines;
}

Joint* VectorPass::addJoint(Joint* p) {
	map<string, Joint*>::iterator it = joints.find(p->getKey());

	if (it != joints.end()) {
		return (Joint*) it->second;
	}

	joints.insert(pair<string, Joint*> (p->getKey(), p));
	return p;
}

void VectorPass::addLine(Joint *start, Joint *end, int power) {
	LineSegment *ls = new LineSegment(start, end, power);

	start = addJoint(start);
	ls->setStart(start);
	start->addConnector(ls);

	end = addJoint(end);
	ls->setEnd(end);
	end->addConnector(ls);

	lines.push_back(ls);
}

list<LineSegment*>::iterator VectorPass::eraseLine(
		list<LineSegment*>::iterator it_ls) {
	LineSegment *ls = *it_ls;
	ls->getStart()->removeConnector(ls);
	ls->getEnd()->removeConnector(ls);

	return lines.erase(it_ls);
}

vector<OnionSkin*> VectorPass::getOnionSkins() {
	return skins;
}

void VectorPass::addOnionSkin(OnionSkin* skin) {
	skins.push_back(skin);
}

void VectorPass::load(const string *file_name) {
	string line;
	ifstream infile(file_name->data(), ios_base::in);
	char first;
	int power, x, y;
	int lx, ly;
	int mx, my;
	Joint *start;
	Joint *end;

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
					addLine(new Joint(lx, ly), new Joint(mx, my), power);
				}
				break;
			case 'P': // power
				if (sscanf((char *) line.data() + 1, "%d", &x) == 1) {
					power = x;
				}
				break;
			case 'L': // line
				if (sscanf((char *) line.data() + 1, "%d,%d", &y, &x) == 2) {
					start = new Joint(lx, ly);
					end = new Joint(x, y);
					addLine(start, end, power);
					lx = x;
					ly = y;
				}
				break;
			}
		}
	}
	infile.close();
}

void VectorPass::write(const string* file_name) {
	list<LineSegment*> skin_segm;
	LineSegment* ls;
	list<LineSegment*>::reverse_iterator it_i;

	ofstream outfile;
	outfile.open(file_name->data(), ofstream::out | ofstream::trunc);

	printf("Skins: %d\n", skins.size());

	unsigned int i;
	for (i = skins.size() - 1; i >= 0; i--) {
		skin_segm = skins.at(i)->getLineSegments();
		printf("s: %d\n", skin_segm.size());

		for (it_i = skin_segm.rbegin(); it_i != skin_segm.rend(); it_i++) {
			ls = *it_i;
			if (it_i != skin_segm.rbegin())
				ls->invertDirection();
			if (ls != NULL) {
				//print_line(ls);
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
			}
		}
	}
	outfile << "X\n";
	outfile.close();

}

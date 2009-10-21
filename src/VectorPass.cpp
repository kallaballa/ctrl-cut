/*
 * VectorCut.cpp
 *
 *  Created on: 04.10.2009
 *      Author: amir
 */
#include <fstream>
#include <iostream>
#include "LaserJob.h"

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

void VectorPass::addOnionSkin(OnionSkin* skin) {
	skins.push_back(skin);
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

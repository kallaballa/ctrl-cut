/*
 * VectorCut.cpp
 *
 *  Created on: 04.10.2009
 *      Author: amir
 */
#include <fstream>
#include <iostream>
#include "LaserJob.h"
#include "boost/format.hpp"

using namespace boost;

Joint* VectorPass::addJoint(Joint* p) {
	JointMap::iterator it = joints.find(p->getKey());

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

LineSegmentList::iterator VectorPass::eraseLine(LineSegmentList::iterator it_ls) {
	LineSegment *ls = *it_ls;
	ls->getStart()->removeConnector(ls);
	ls->getEnd()->removeConnector(ls);

	return lines.erase(it_ls);
}

void VectorPass::addOnionSkin(OnionSkin* skin) {
	skins.push_back(skin);
}

void VectorPass::write(const string &file_name) {
	LineSegmentList skin_segm;
	LineSegment* ls;
	LineSegmentList::reverse_iterator it_i;

	ofstream outfile;
	outfile.open(file_name.c_str(), ofstream::out | ofstream::trunc);

	printf("Skins: %d\n", skins.size());

	for (int i = skins.size() - 1; i >= 0; i--) {
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

/**
 *
 */
void VectorPass::serializeTo(ostream& out) {
	bool first = true;
	int power = 100;

	out << V_INIT << SEP;
	out << format(V_FREQUENCY) % this->lconf->vector_freq << SEP;
	out << format(V_POWER) % this->lconf->vector_power << SEP;
	out << format(V_SPEED) % this->lconf->vector_speed << SEP;

	LineSegment* ls;
	//TODO: Recognize contiguous polylines and avoid unnecessary PU/PD
	for (LineSegmentList::iterator it_s = this->lines.begin(); it_s
			!= this->lines.end(); it_s++) {
		ls = *it_s;

		if (ls->getPower() != power) {
			power = ls->getPower();
			int epower;
			if (first) {
				out << SEP << HPGL_PEN_UP_INIT;
			} else {
				out << SEP << HPGL_PEN_UP;
			}

			epower = (power * this->lconf->vector_power + 50) / 100;
			if (this->lconf->vector_speed && this->lconf->vector_speed < 100) {
				int espeed = this->lconf->vector_speed;
				int efreq = this->lconf->vector_freq;
				if (epower && power < 100) {
					int r;
					int q;
					r = 10000 / power; // power, up to set power level (i.e. x=100)
					q = 10000 / espeed;
					if (q < r)
						r = q;
					q = 500000 / efreq;
					if (q < r)
						r = q;
					epower = (50 + epower * r) / 100;
					espeed = (50 + espeed * r) / 100;
					efreq = (50 + espeed * r) / 100;
				}

				out << SEP << format(V_SPEED) % espeed;
				out << SEP << format(V_FREQUENCY) % efreq << SEP;
			}
			out << format(V_POWER) % epower << SEP;
		}

		if (first) {
			out << HPGL_PEN_UP_INIT;
		} else {
			out << HPGL_PEN_UP;
		}

		out << format("%d,%d") % (this->lconf->basex + ls->getStart()->getX()
				+ HPGLX)
				% (this->lconf->basey + ls->getStart()->getY() + HPGLY);
		out << SEP << HPGL_PEN_DOWN;
		out << format("%d,%d") % (this->lconf->basex + ls->getEnd()->getX()
				+ HPGLX) % (this->lconf->basey + ls->getEnd()->getY() + HPGLY);
		out << SEP;
		first = false;
	}

	out << SEP << HPGL_END << PCL_SECTION_END << HPGL_PEN_UP;
}


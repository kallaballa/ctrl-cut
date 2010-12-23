/*
 * Renderer.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: amir hassan
 */

#include "Renderer.h"
#include "boost/format.hpp"

using boost::format;

Renderer::Renderer(LaserConfig *lconf) {
	this->lconf = lconf;
}

Renderer::~Renderer() {
	// TODO Auto-generated destructor stub
}

void Renderer::renderCut(Cut *cut, ostream &out) {
	bool first = true;
	bool writingPolyline = false;

	out << V_INIT << SEP;
	out << format(V_POWER) % this->lconf->vector_power << SEP;
	out << format(V_SPEED) % this->lconf->vector_speed << SEP;
	out << format(V_FREQUENCY) % this->lconf->vector_freq << SEP;

	Edge* ls;
	int beginX = -1, beginY = -1;
	int lastX = -1, lastY = -1;
	int lastPower = this->lconf->vector_power;
	for (VecPolyline::iterator it_p = cut->polylines.begin(); it_p
			!= cut->polylines.end(); it_p++) {
		Polyline *p = *it_p;
		for (vector<Edge*>::iterator it_s = p->edges.begin(); it_s != p->edges.end(); it_s++) {
			ls = *it_s;

			int power = ls->getPower();
			if (power != lastPower) {
				if (writingPolyline) {
					out << SEP;
					writingPolyline = false;
				}
				int epower = (power * this->lconf->vector_power + 50) / 100;
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

			int startX = this->lconf->basex + ls->getStart()->getX() + HPGLX;
			int startY = this->lconf->basey + ls->getStart()->getY() + HPGLY;
			int endX = this->lconf->basex + ls->getEnd()->getX() + HPGLX;
			int endY = this->lconf->basey + ls->getEnd()->getY() + HPGLY;

			//     if (beginX < 0) {
			//       beginX = startX;
			//       beginY = startY;
			//     }

			// After a power change, always issue a PU, even if the current
			// coordinate doesn't change.
			if (lastX != startX || lastY != startY || lastPower != power) {
				if (first) {
					// FIXME: This is to emulate the LT bug in the Epilog drivers:
					// Check if any clipping has been done in any of the passes, and
					// inject the stray "LT" string. This has no function, just for bug compatibility
					// of the output files. See corresponding FIXME in LaserJob.cpp.
					if (!cut->wasClipped())
						out << HPGL_LINE_TYPE;
					first = false;
				} else {
					out << SEP;
				}
				out << HPGL_PEN_UP;
				beginX = startX;
				beginY = startY;
				out << format("%d,%d") % startX % startY << SEP;
				out << HPGL_PEN_DOWN << format("%d,%d") % endX % endY;
				writingPolyline = true;
			}
			// ..else continue on polyline
			else {
				out << format(",%d,%d") % endX % endY;
			}

			lastX = endX;
			lastY = endY;
			lastPower = power;

			// FIXME: This is a temporary hack to emulate the Epilog Windows driver,
			// which appears to repeat the first vertex in a closed polyline twice at the end.
			if (beginX == lastX && beginY == lastY) {
				out << format(",%d,%d") % beginX % beginY;
			}
		}
	}

	out << SEP << HPGL_END << PCL_SECTION_END << HPGL_PEN_UP;
}

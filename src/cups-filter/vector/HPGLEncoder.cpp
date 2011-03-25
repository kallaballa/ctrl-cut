/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "HPGLEncoder.h"
#include "Polyline.h"
#include "Edge.h"
#include "Vertex.h"
#include "Cut.h"
#include "boost/format.hpp"

using boost::format;

HPGLEncoder::HPGLEncoder(LaserConfig *lconf) {
  this->lconf = lconf;
}

HPGLEncoder::~HPGLEncoder() {
  // TODO Auto-generated destructor stub
}

void HPGLEncoder::encode(Cut *cut, ostream &out) {
  bool first = true;
  bool writingPolyline = false;

  out << V_INIT << SEP;
  out << format(V_POWER) % this->lconf->vector_power << SEP;
  out << format(V_SPEED) % this->lconf->vector_speed << SEP;
  out << format(V_FREQUENCY) % this->lconf->vector_freq << SEP;

  int beginX = -1, beginY = -1;
  int lastX = -1, lastY = -1;
  int lastPower = this->lconf->vector_power;
  for (Cut::iterator it_p = cut->begin(); it_p != cut->end(); it_p++) {
    Polyline *p = *it_p;
    for (Polyline::iterator it_s = p->begin(); it_s != p->end(); it_s++) {
      const Edge &edge = **it_s;

      int power = (edge.power != -1) ? edge.power : this->lconf->vector_power;
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

      int startX = this->lconf->basex + edge[0][0];
      int startY = this->lconf->basey + edge[0][1];
      int endX = this->lconf->basex + edge[1][0];
      int endY = this->lconf->basey + edge[1][1];

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

  if (writingPolyline) out << SEP;
  out << HPGL_END;
}

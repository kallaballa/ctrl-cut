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
#include "HPGLEncoder.hpp"
#include "util/PJL.hpp"
#include "cut/Cut.hpp"
#include "cut/operations/Deonion.hpp"
#include "cut/operations/NearestPathSorting.h"
#include "cut/operations/Translate.hpp"
#include "svg/SvgPlot.hpp"
#include <boost/format.hpp>

using boost::format;

void HPGLEncoder::encode(std::ostream &out, Cut& encodee) {
  bool firstOperation = true;
  bool writingPolyline = false;

  int power_set = encodee.get(CutSettings::CPOWER);
  int speed_set = encodee.get(CutSettings::CSPEED);
  int freq_set = encodee.get(CutSettings::FREQUENCY);

  out << V_INIT << SEP;
  out << format(V_POWER) % power_set << SEP;
  out << format(V_SPEED) % speed_set << SEP;
  out << format(V_FREQUENCY) % freq_set << SEP;

  int beginX = -1, beginY = -1;
  int lastX = -1, lastY = -1;
  int lastPower = power_set;

  BOOST_FOREACH(const Segment& seg, segments(encodee)) {
    int power = power_set;// (seg.get(S_SET::S_POWER) != 0) ? seg.get(S_SET::S_POWER) : power_set;
    if (power != lastPower) {
      if (writingPolyline) {
        out << SEP;
        writingPolyline = false;
      }
      int epower = (power * power_set + 50) / 100;
      if (speed_set && speed_set < 100) {
        int espeed = speed_set;
        int efreq = freq_set;
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

    int startX = seg[0][0];
    int startY = seg[0][1];
    int endX = seg[1][0];
    int endY = seg[1][1];

    //     if (beginX < 0) {
    //       beginX = startX;
    //       beginY = startY;
    //     }

    // After a power change, always issue a PU, even if the current
    // coordinate doesn't change.
    if (lastX != startX || lastY != startY || lastPower != power) {
      if (firstOperation) {
        // FIXME: This is to emulate the LT bug in the Epilog drivers:
        // Check if any clipping has been done in any of the passes, and
        // inject the stray "LT" string. This has no function, just for bug compatibility
        // of the output files. See corresponding FIXME in LaserJob.cpp.
/*REFACTOR          if (!model.wasClipped()) */
        out << HPGL_LINE_TYPE;
        firstOperation = false;
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
    if (beginX == lastX && beginY == lastY)
      out << format(",%d,%d") % beginX % beginY;
  }

  if (writingPolyline) out << SEP;
  out << HPGL_END;
}

/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "HPGLEncoder.hpp"
#include "util/PJL.hpp"
#include "cut/Cut.hpp"
#include "cut/operations/Deonion.hpp"
#include "cut/operations/Translate.hpp"
#include <boost/format.hpp>
#include <cut/operations/NearestPathSorting.hpp>

using boost::format;

void HPGLEncoder::encode(std::ostream &out, Cut& encodee) {
  bool firstOperation = true;
  bool writingPolyline = false;

  int cnt = 0;

  int power_set = encodee.get(CutSettings::CPOWER);
  int speed_set = encodee.get(CutSettings::CSPEED);
  int freq_set = encodee.get(CutSettings::FREQUENCY);

/* out << V_INIT << SEP;
  out << format(V_POWER) % power_set << SEP;
  out << format(V_SPEED) % speed_set << SEP;
  out << format(V_FREQUENCY) % freq_set << SEP;*/

  int beginX = -1, beginY = -1;
  int lastX = -1, lastY = -1;

  for(const SegmentPtr segPtr : segments(encodee)) {
    if (cnt % 1000 == 0) {
      out << HPGL_END;
      out << V_INIT << SEP;
      out << format(V_POWER) % power_set << SEP;
      out << format(V_SPEED) % speed_set << SEP;
      out << format(V_FREQUENCY) % freq_set << SEP;
      firstOperation = true;
    }

    Segment& seg = *segPtr.get();
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
    if (lastX != startX || lastY != startY) {
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

    // FIXME: This is a temporary hack to emulate the Epilog Windows driver,
    // which appears to repeat the first vertex in a closed polyline twice at the end.
    if (beginX == lastX && beginY == lastY)
      out << format(",%d,%d") % beginX % beginY;

    ++cnt;
  }

  if (writingPolyline) out << SEP;
  out << HPGL_END;
}

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
  int beginX = -1, beginY = -1;
  int lastX = -1, lastY = -1;

  size_t cnt = 0;
  for(const SegmentPtr segPtr : segments(encodee)) {
    if(cnt % 100 == 0) {
      cnt = 0;
      if(!firstOperation) {
        if (writingPolyline)
          out << SEP;
        out << HPGL_END;
        out << SEP;
      }
      out << HPGL_START;
      out << V_INIT << SEP;
      out << format(V_POWER) % power_set << SEP;
      out << format(V_SPEED) % speed_set << SEP;
      out << format(V_FREQUENCY) % freq_set; // omit separator;

      firstOperation = false;

      lastX = -1;
      lastY = -1;
    }
    Segment& seg = *segPtr.get();
    int startX = seg[0][0];
    int startY = seg[0][1];
    int endX = seg[1][0];
    int endY = seg[1][1];

    if (lastX != startX || lastY != startY) {
      beginX = startX;
      beginY = startY;

      out << SEP << HPGL_PEN_UP;
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

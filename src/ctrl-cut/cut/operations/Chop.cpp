/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Chop.hpp"
#include "util/Logger.hpp"
#include "cut/geom/Geometry.hpp"
#include "cut/geom/algorithms/Algorithms.hpp"

void chop(const Route& src, Route& sink, double maxLength) {
  for(SegmentPtr seg : segments(src)) {
    if(double len = seg->length() > maxLength) {
      Point lastPoint = seg->first;
      Point currentPoint;
      for(size_t i = 0; i < (len / maxLength); i++) {
        Segment newSeg;
        double t = i/(len/maxLength);
        currentPoint.x = seg->first.x * (1-t) + seg->second.x * t;
        currentPoint.y = seg->first.y * (1-t) + seg->second.y * t;
        append(sink, Segment(lastPoint, currentPoint));
        lastPoint = currentPoint;
      }
      if(fmod(len, maxLength) > 0) {
        append(sink, Segment(lastPoint, seg->second));
      }
    } else {
      append(sink, *seg);
    }
  }

  LOG_INFO_STR("chop");
  LOG_DEBUG(sink.size());
}

/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Chop.hpp"
#include "util/Logger.hpp"
#include "cut/geom/Geometry.hpp"
#include "cut/geom/algorithms/Algorithms.hpp"
#include <cassert>

void chop(const Route& src, Route& sink, Coord_t maxLength) {
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

void chop(const Route& src, Route& sink, Coord_t maxAmplitudeX, Coord_t maxAmplitudeY) {
  for (SegmentPtr seg : segments(src)) {
    Point d = seg->second - seg->first;

    if ((std::fabs(d.x) > maxAmplitudeX) || (std::fabs(d.y) > maxAmplitudeY)) {
      double ratX = std::fabs(d.x) / maxAmplitudeX;
      double ratY = std::fabs(d.y) / maxAmplitudeY;

      double maxRat = std::max(ratX, ratY);

      Point lastPoint = seg->first;
      Point currentPoint;
      for (size_t i = 0; i < maxRat; i++) {
        currentPoint.x = (seg->first.x + (d.x * i / maxRat));
        currentPoint.y = (seg->first.y + (d.y * i / maxRat));
        add(sink, Segment(lastPoint, currentPoint));
        lastPoint = currentPoint;
      }
      add(sink, Segment(lastPoint, seg->second));
    } else {
      add(sink, *seg);
    }
  }
  LOG_INFO_STR("chop");
  LOG_DEBUG(sink.size());
}

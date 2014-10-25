/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Clip.hpp"
#include "util/Logger.hpp"
#include "cut/geom/Geometry.hpp"
#include "util/Measurement.hpp"
#include "cut/geom/algorithms/Algorithms.hpp"
#include <algorithm>

/*
 * Clips segments agains a box
 */
void clip(Route& src, Route& sink, const Box& bounds) {
  LOG_DEBUG(src.size());

  for(const SegmentPtr segPtr : segments(src)) {
    Segment& seg = *segPtr.get();
    double width = bounds.width();
    double height = bounds.height();

    Segment leftBedBorder(Point(0, 0), Point(0, height - 1));
    Segment bottomBedBorder(Point(0, height - 1), Point(width - 1, height - 1));
    Segment rightBedBorder(Point(width - 1, height - 1), Point(width - 1, 0));
    Segment topBedBorder(Point(width - 1, 0), Point(0, 0));

    Point intersection;
    Segment clipped = seg;

    if (clipped.first.x < 0 || clipped.second.x < 0) {
      // out of bounds;
      if (clipped.first.x < 0 && clipped.second.x < 0) {
        continue;
      }

      if (intersects(clipped, leftBedBorder, intersection) == ALIGN_INTERSECT) {
        if (clipped.first.x >= clipped.second.x)
          clipped.second = clipped.first;

        clipped.first = intersection;
        intersection = Point();
      }
    }

    if (clipped.first.y < 0 || clipped.second.y < 0) {
      if (clipped.first.y < 0 && clipped.second.y < 0) {
        continue;
      }

      if (intersects(clipped, topBedBorder, intersection) == ALIGN_INTERSECT) {
        if (clipped.first.y >= clipped.second.y)
          clipped.second = clipped.first;

        clipped.first = intersection;

        intersection = Point();
      }
    }

    if (greater_than(clipped.first.x, width - 1)
        || greater_than(clipped.second.x, width - 1)) {
      if (greater_than(clipped.first.x, width - 1)
          && greater_than(clipped.second.x, width - 1)) {
        continue;
      }

      if (intersects(clipped, rightBedBorder, intersection) == ALIGN_INTERSECT) {
        if (clipped.first.x <= clipped.second.x)
          clipped.second = clipped.first;

        clipped.first = intersection;

        intersection = Point();
      }
    }

    if (clipped.first.y > height - 1 || clipped.second.y > height - 1) {
      if (clipped.first.y > height - 1 && clipped.second.y > height - 1) {
        continue;
      }
      if (intersects(clipped, bottomBedBorder, intersection) == ALIGN_INTERSECT) {
        if (clipped.first.y <= clipped.second.y)

          clipped.second = clipped.first;
        clipped.first = intersection;
      }
    }

    add(sink, clipped);
  }
  LOG_DEBUG(sink.size());
}

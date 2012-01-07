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


#ifndef CLIP_HPP_
#define CLIP_HPP_

#include "util/Logger.hpp"
#include "cut/geom/Geometry.hpp"

#include <boost/foreach.hpp>

/*
 * Clips segments agains a box
 */
template<
  typename TsegmentInputRange,
  typename TsegmentOutputIterator
>
class Clip {
private:
  TsegmentInputRange src;
  TsegmentOutputIterator sink;
  Box bounds;
public:
  Clip(TsegmentInputRange src, TsegmentOutputIterator sink, Box& bounds) :
    src(src), sink(sink), bounds(bounds) {
  }

  Clip(const Clip& other) :
    src(other.src), sink(other.sink), bounds(other.bounds)
  {}

  void operator()() {
    BOOST_FOREACH(const Segment& seg, src) {
      double width = bounds.width();
      double height = bounds.height();

      Segment leftBedBorder(Point(0, 0),Point(0, height-1));
      Segment bottomBedBorder(Point(0, height-1),Point(width-1, height-1));
      Segment rightBedBorder(Point(width-1, height-1),Point(width-1, 0));
      Segment topBedBorder(Point(width-1, 0),Point(0, 0));

      Point intersection;
      Segment clipped = seg;

      if(clipped.first.x < 0 || clipped.second.x < 0) {
        // out of bounds;
        if(clipped.first.x < 0 && clipped.second.x < 0) {
          continue;
        }

        if(intersects(clipped, leftBedBorder, intersection) == ALIGN_INTERSECT) {
          if(clipped.first.x < clipped.second.x)
            clipped.first = intersection;
          else
            clipped.second = clipped.first;
            clipped.first = intersection;

          intersection = Point();
        }
      }

      if(clipped.first.y < 0 || clipped.second.y < 0) {
        if(clipped.first.y < 0 && clipped.second.y < 0) {
          return;
        }

        if(intersects(clipped, topBedBorder, intersection) == ALIGN_INTERSECT) {
          if(clipped.first.y < clipped.second.y)
            clipped.first = intersection;
          else
            clipped.second = clipped.first;
            clipped.first = intersection;

          intersection = Point();
        }
      }

      if(greater_than(clipped.first.x,width - 1) || greater_than(clipped.second.x,width - 1)) {
        if(greater_than(clipped.first.x, width - 1) && greater_than(clipped.second.x,width - 1)) {
          return;
        }

        if(intersects(clipped, rightBedBorder, intersection) == ALIGN_INTERSECT) {
          if(clipped.first.x > clipped.second.x)
            clipped.first = intersection;
          else
            clipped.second = clipped.first;
            clipped.first = intersection;

          intersection = Point();
        }
      }

      if(clipped.first.y > height - 1 || clipped.second.y > height - 1) {
        if(clipped.first.y > height - 1 && clipped.second.y > height - 1) {
          return;
        }
        if(intersects(clipped, bottomBedBorder, intersection) == ALIGN_INTERSECT) {
          if(clipped.first.y > clipped.second.y)
            clipped.first = intersection;
          else
            clipped.second = clipped.first;
            clipped.first = intersection;
        }
      }
      *sink++ = clipped;
    }
  }
};

template<
  typename TsegmentInputRange,
  typename TsegmentOutputIterator
>
void clip(TsegmentInputRange segmentSrc, TsegmentOutputIterator segmentSink, Box box) {
  Clip<TsegmentInputRange, TsegmentOutputIterator> clipper(segmentSrc,segmentSink,box);
  clipper();
}
#endif /* CLIP_HPP_ */

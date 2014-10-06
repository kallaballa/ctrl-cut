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

#ifndef MERGE_H_
#define MERGE_H_

#include "Cut.hpp"

inline void merge(Cut& src, Cut& sink) {
  namespace tl = boost::geometry::strategy::transform;
  Point pFrom = src.get(CutSettings::CPOS);
  Point pInto = sink.get(CutSettings::CPOS);

  if(pFrom != pInto) {
    tl::translate_transformer<Point, Point> transformer(pFrom.x + (pInto.x * -1), pFrom.y + (pInto.y * -1));

    for(const SegmentPtr seg : segments(src)) {
      Segment translated;
      boost::geometry::transform(*seg.get(), translated, transformer);
      append(sink, translated);
    }
  } else {
    for(const SegmentPtr seg : segments(src)) {
      append(sink, *seg.get());
    }
  }

  sink.prepare();
}

#endif

/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
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

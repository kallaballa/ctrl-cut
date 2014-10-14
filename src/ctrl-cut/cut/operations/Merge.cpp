/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "Merge.hpp"

void merge(Cut& src, Cut& sink) {
  namespace tl = boost::geometry::strategy::transform;
  Point pFrom = src.get(CutSettings::CPOS);
  Point pInto = sink.get(CutSettings::CPOS);

  if(pFrom != pInto) {
    tl::translate_transformer<Coord_t, 2, 2> transformer(pFrom.x + (pInto.x * -1), pFrom.y + (pInto.y * -1));

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
}


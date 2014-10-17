/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "Merge.hpp"

void merge(const Cut& src, Cut& sink) {
  namespace tl = boost::geometry::strategy::transform;

  Cut srcTrans = make_from(src);
  Cut sinkTrans = make_from(sink);

  translateTo(src, srcTrans, src.get(CutSettings::CPOS));
  translateTo(sink, sinkTrans, sink.get(CutSettings::CPOS));
  for(const SegmentPtr seg : segments(srcTrans)) {
    append(sinkTrans, *seg.get());
  }
  sink = sinkTrans;
  sink.put(CutSettings::CPOS, Point());
}


/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Explode.hpp"
#include "util/Logger.hpp"
#include "cut/geom/algorithms/Append.hpp"
#include <CGAL/Cartesian.h>
#include <CGAL/MP_Float.h>
#include <CGAL/Quotient.h>
#include <CGAL/Arr_segment_traits_2.h>
#include <CGAL/Sweep_line_2_algorithms.h>
#include <list>

typedef CGAL::Quotient<CGAL::MP_Float>                  NT;
typedef CGAL::Cartesian<NT>                             Kernel;
typedef Kernel::Point_2                                 Point_2;
typedef CGAL::Arr_segment_traits_2<Kernel>              Traits_2;
typedef Traits_2::Curve_2                               Segment_2;

void explode(const Route& src, Route& sink) {
  std::vector<Segment_2> segs;
  for(const SegmentPtr& seg : segments(src)) {
    if(seg->first != seg->second)
      segs.push_back(Segment_2(Point_2(seg->first.x, seg->first.y), Point_2(seg->second.x, seg->second.y)));
  }
  std::list<Segment_2>   sub_segs;

  CGAL::compute_subcurves(segs.data(), segs.data() + segs.size(), std::back_inserter(sub_segs));

  for(const Segment_2& subseg : sub_segs) {
    double x1 = CGAL::to_double(subseg.source()[0]);
    double y1 = CGAL::to_double(subseg.source()[1]);
    double x2 = CGAL::to_double(subseg.target()[0]);
    double y2 = CGAL::to_double(subseg.target()[1]);
    append(sink, Segment(Point(x1,y1), Point(x2,y2)));
  }
}

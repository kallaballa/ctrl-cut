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
#include "LaosEncoder.h"
#include "vector/model/CutModel.h"
#include "vector/graph/Traverse.h"
#include "boost/format.hpp"

using boost::format;

#define CONVERT(p) trunc(55.92105557*72.0/this->lconf->resolution*p)

void LaosEncoder::encode(CutModel& model, std::ostream &out) const {
  StringList route;
  make_route(route, model);

  out << "3 " << this->lconf->vector_speed << " ";

  for (StringList::const_iterator it_ss = route.begin(); it_ss != route.end(); ++it_ss) {
    const SegmentString& segString = *(*it_ss);
    const Point* segmentFront = segString.frontPoints();
    out << format("0 %d %d ") % CONVERT(segmentFront->x) % CONVERT(segmentFront->y);

    for (SegmentString::SegmentConstIter it_s = segString.beginSegments(); it_s != segString.endSegments(); ++it_s) {
      const Segment &seg = **it_s;
      int startX = this->lconf->basex + seg[0][0];
      int startY = this->lconf->basey + seg[0][1];
      int endX = this->lconf->basex + seg[1][0];
      int endY = this->lconf->basey + seg[1][1];

      out << format("1 %d %d ") % CONVERT(endX) % CONVERT(endY);
    }
  }
}

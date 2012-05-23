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

#ifndef DISTANCEGRAPH_H_
#define DISTANCEGRAPH_H_

#include "WeightedGraph.hpp"

template<typename Tgeom>
class DistanceGraph : public WeightedGraph<Segment> {
public:
  typedef WeightedGraph<Segment> _Base;

  DistanceGraph() :
    _Base()
  {}

  DistanceGraph(const DistanceGraph& graph) :
    _Base(graph)
  {}

  DistanceGraph(v_size size) :
    _Base(size)
  {}

  virtual void add(const Tgeom& geom, double distance = -1) {
    const Point& front = geom.front();
    const Point& back = geom.back();

    if(distance < 0)
      distance = front.distance(back);

    _Base::add(Segment(geom.front(), geom.back()), distance);
  }
};


#endif

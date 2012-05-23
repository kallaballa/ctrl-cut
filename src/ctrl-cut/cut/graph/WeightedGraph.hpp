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

#ifndef WEIGHTEDGRAPH_HPP_
#define WEIGHTEDGRAPH_HPP_

#include "GeometryGraph.hpp"
#include <boost/foreach.hpp>

struct WeightProperty {
  double weight;

  WeightProperty(const double& weight) : weight(weight) {}
};

template<typename Tgeom>
class WeightedGraph :
  public GeometryGraph<
    boost::setS,
    boost::vecS,
    boost::undirectedS,
    Point,
    WeightProperty
> {
public:
  typedef GeometryGraph<boost::setS,boost::vecS,boost::undirectedS,Point,WeightProperty> _Base;

  WeightedGraph() :
    _Base() {}
  WeightedGraph(const WeightedGraph& graph) :
    _Base(graph) {}
  WeightedGraph(size_t size) :
    _Base(size){}
  virtual ~WeightedGraph()
  {}

  virtual void add(const Tgeom& geom, double weight = 0) {
    bool first = true;
    const Point front;
    const Point back;

    BOOST_FOREACH(const Point& p, geom) {
      if(first) {
        front = p;
        first = false;
      } else {
        back = p;
        boost::add_edge(addVertex(geom.front()), addVertex(geom.back()), WeightProperty(weight), *this);
        front = back;
      }
    }
  }
};

#endif /* WEIGHTEDGRAPH_HPP_ */

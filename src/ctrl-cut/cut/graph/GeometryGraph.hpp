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

#ifndef GEOMETRYGRAPH_HPP_
#define GEOMETRYGRAPH_HPP_

#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <cut/graph/Algorithms.hpp>
#include <cut/graph/SegmentGraph.hpp>

template<
  class TedgeList = boost::setS,
  class TvertexList = boost::vecS,
  class TdirectedS = boost::undirectedS,
  typename TvertexProperty = boost::no_property,
  typename TedgeProperty = boost::no_property
>
class GeometryGraph :
  public boost::adjacency_list<
    TedgeList,
    TvertexList,
    TdirectedS,
    TvertexProperty,
    TedgeProperty
  > {
public:
  typedef boost::adjacency_list<
      TedgeList,
      TvertexList,
      TdirectedS,
      TvertexProperty,
      TedgeProperty
    > _Base;
  typedef GeometryGraph<TedgeList, TvertexList, TdirectedS, TvertexProperty, TedgeProperty> _Self;

  typedef typename boost::graph_traits<_Self>::vertex_descriptor Vertex;
  typedef typename boost::graph_traits<_Self>::edge_descriptor Edge;
  typedef typename boost::graph_traits<_Self>::vertices_size_type v_size;
  typedef std::vector<std::vector< typename _Self::Edge > > Embedding;

  GeometryGraph() : _Base(){}
  GeometryGraph(const GeometryGraph& graph) : _Base(graph) {}
  GeometryGraph(v_size size) : _Base(size) {}

  virtual Vertex addVertex(const TvertexProperty& geom) {
    return boost::add_vertex(geom, *this);
  }

  //concept create(geom,prop) && add(geom,prop)
};

template<typename TmultiPointRange, typename Tgraph>
void build(TmultiPointRange src, Tgraph& graph) {
  //BOOST_CONCEPT_ASSERT((SegmentInputIterator<TgeomIn>));

  BOOST_FOREACH(const typename TmultiPointRange::value_type pointRange, src) {
    graph.createEdge(pointRange);
  }
}


#endif /* GEOMETRYGRAPH_HPP_ */

/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef COMPLETEDISTANCEGRAPH_HPP_
#define COMPLETEDISTANCEGRAPH_HPP_

#include "cut/geom/Geometry.hpp"
#include "cut/geom/Path.hpp"
#include "GeometryGraph.hpp"
#include <boost/foreach.hpp>

struct DistanceProperty {
  double distance;

  DistanceProperty(const double& distance) : distance(distance) {}
};

template<typename Tgeom>
class CompleteDistanceGraph :
  public GeometryGraph<
    boost::setS,
    boost::vecS,
    boost::undirectedS,
    Tgeom,
    DistanceProperty
> {
public:
  typedef GeometryGraph<boost::setS,boost::vecS,boost::undirectedS,Tgeom,DistanceProperty> _Base;
  typedef typename boost::graph_traits<_Base>::vertex_descriptor Vertex;
  typedef typename boost::graph_traits<_Base>::edge_descriptor Edge;

  CompleteDistanceGraph() :
    _Base() {}
  CompleteDistanceGraph(const CompleteDistanceGraph& graph) :
    _Base(graph) {}
  CompleteDistanceGraph(size_t size) :
    _Base(size){}
  virtual ~CompleteDistanceGraph()
  {}

  virtual Vertex addVertex(const Tgeom& geom) {
    return _Base::addVertex(geom);
  }

  void makeComplete() {
    BOOST_FOREACH(Vertex i, boost::vertices(*this)) {
      BOOST_FOREACH(Vertex j, boost::vertices(*this)) {
        if(i != j && !boost::edge(i, j,*this).second) {
          const Path& ip = (*this)[i];
          const Path& jp = (*this)[j];

          boost::add_edge(i, j, DistanceProperty(ip.front().distance(jp.front())), *this);
          boost::add_edge(i, j, DistanceProperty(ip.back().distance(jp.front())), *this);
          boost::add_edge(i, j, DistanceProperty(ip.front().distance(jp.back())), *this);
          boost::add_edge(i, j, DistanceProperty(ip.back().distance(jp.back())), *this);
        }
      }
    }
  }
};

#endif /* COMPLETEDISTANCEGRAPH_HPP_ */

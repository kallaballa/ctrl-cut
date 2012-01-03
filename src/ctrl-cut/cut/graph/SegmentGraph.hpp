#ifndef SEGMENTGRAPH_H_
#define SEGMENTGRAPH_H_

#include "GeometryGraph.hpp"

class SegmentProperty : public Segment {
public:
  bool owned;
  SegmentProperty() : Segment(), owned(false)  {}
  SegmentProperty(const Segment& s) : Segment(s), owned(false) {}
};

class SegmentGraph :
  public GeometryGraph<
    boost::setS,
    boost::vecS,
    boost::undirectedS,
    Point,
    SegmentProperty
> {
private:
    typedef GeometryGraph<boost::setS,boost::vecS,boost::undirectedS,Point,SegmentProperty> _Base;
public:
  SegmentGraph() :
    _Base()
  {}
  SegmentGraph(const SegmentGraph& graph) :
    _Base(graph)
  {}
  SegmentGraph(_Base::v_size size) :
    _Base(size)
  {}

  void create(const Segment& seg) {
    SegmentGraph::Vertex inV = addVertex(seg.first);
    SegmentGraph::Vertex outV = addVertex(seg.second);
    boost::add_edge(inV, outV, SegmentProperty(seg), *this);
  }

  void add(const Segment& seg) {
    SegmentGraph::Vertex inV = addVertex(seg.first);
    SegmentGraph::Vertex outV = addVertex(seg.second);
    boost::add_edge(inV, outV, SegmentProperty(seg), *this);
  }
};
#endif

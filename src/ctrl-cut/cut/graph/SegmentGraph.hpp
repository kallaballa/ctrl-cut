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
    std::map<Point, Vertex> index;
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

  Vertex addVertex(const Point& p) {
    std::map<Point, Vertex>::iterator it = index.find(p);
    if(it == index.end()) {
      Vertex v = boost::add_vertex(p, *this);
      index[p] = v;
      return v;
    } else
      return (*it).second;
  }

  void create(const Segment& seg) {
    SegmentGraph::Vertex inV = SegmentGraph::addVertex(seg.first);
    SegmentGraph::Vertex outV = SegmentGraph::addVertex(seg.second);
    boost::add_edge(inV, outV, SegmentProperty(seg), *this);
  }

  void add(const Segment& seg) {
    SegmentGraph::Vertex inV = SegmentGraph::addVertex(seg.first);
    SegmentGraph::Vertex outV = SegmentGraph::addVertex(seg.second);
    boost::add_edge(inV, outV, SegmentProperty(seg), *this);
  }
};

template<typename TmultiPointRange>
void build(TmultiPointRange src, SegmentGraph& graph) {
  //BOOST_CONCEPT_ASSERT((SegmentInputIterator<TgeomIn>));
  MultiSegmentView<TmultiPointRange> msv(src);
  BOOST_FOREACH(const Segment seg, msv) {
    graph.create(seg);
  }
}
#endif


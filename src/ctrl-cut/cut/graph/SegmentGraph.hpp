#ifndef SEGMENTGRAPH_H_
#define SEGMENTGRAPH_H_

#include "GeometryGraph.hpp"

class SegmentProperty : public Segment {
public:
  bool owned;
  SegmentProperty() : Segment(), owned(false)  {}
  SegmentProperty(const Segment& s) : Segment(s), owned(false) {}
};

template<
  class TedgeList = boost::setS,
  class TvertexList = boost::vecS
>
class SegmentGraphImpl :
  public GeometryGraph<
    TedgeList,
    TvertexList,
    boost::undirectedS,
    Point,
    SegmentProperty
> {
private:
    typedef GeometryGraph<TedgeList,TvertexList,boost::undirectedS,Point,SegmentProperty> _Base;
    typedef SegmentGraphImpl<_Base> _Self;
public:
    typedef typename _Base::Vertex Vertex;
    std::map<Point,Vertex> index;
    SegmentGraphImpl() :
    _Base()
  {}
    SegmentGraphImpl(const _Self& graph) :
    _Base(graph)
  {}
    SegmentGraphImpl(typename _Base::v_size size) :
    _Base(size)
  {}

  Vertex addVertex(const Point& p) {
    typename std::map<Point, Vertex>::iterator it = index.find(p);
    if(it == index.end()) {
      Vertex v = boost::add_vertex(p, *this);
      index[p] = v;
      return v;
    } else
      return (*it).second;
  }

  virtual void addSegment(const Segment& seg) {
    Vertex inV = this->addVertex(seg.first);
    Vertex outV = this->addVertex(seg.second);
    boost::add_edge(inV, outV, SegmentProperty(seg), *this);
  }
};

  typedef SegmentGraphImpl<boost::vecS, boost::vecS> SegmentGraph;
#endif


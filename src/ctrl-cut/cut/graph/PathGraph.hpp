#ifndef POINTGRAPH_H_
#define POINTGRAPH_H_

#include "GeometryGraph.hpp"
#include "cut/geom/Path.hpp"

class PathProperty : public Path {
public:
  bool owned;
  PathProperty() : Path(), owned(false)  {}
  PathProperty(const Path& p) : Path(p), owned(false) {}
};

class PathGraph:
  public GeometryGraph<
    boost::setS,
    boost::setS,
    boost::undirectedS,
    Point,
    PathProperty
> {
private:
    typedef GeometryGraph<boost::setS,boost::setS,boost::undirectedS,Point,PathProperty> _Base;
    std::map<Point, Vertex> index;
public:
    PathGraph() :
    _Base()
  {}
    PathGraph(const PathGraph& graph) :
    _Base(graph)
  {}
    PathGraph(_Base::v_size size) :
    _Base(size)
  {}

  void addPath(const Path& path) {
    PathGraph::Vertex inV = boost::add_vertex(path.front(), *this);
    PathGraph::Vertex outV = boost::add_vertex(path.back(), *this);
    boost::add_edge(inV, outV, PathProperty(path), *this);
  }
};

#endif


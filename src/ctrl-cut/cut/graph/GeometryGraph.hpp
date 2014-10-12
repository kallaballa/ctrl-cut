
#ifndef GEOMETRYGRAPH_HPP_
#define GEOMETRYGRAPH_HPP_

#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <cut/graph/Algorithms.hpp>

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

  virtual void add(const TvertexProperty& geom) {
    addVertex(geom);
  }
};

#endif /* GEOMETRYGRAPH_HPP_ */

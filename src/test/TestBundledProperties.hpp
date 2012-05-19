#ifndef TESTBUNDLEDPROPERTIES_H_
#define TESTBUNDLEDPROPERTIES_H_

#include <boost/graph/properties.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>

class TestBundledProperties {
  struct VertexProperty {
    VertexProperty() {}
  };

  struct EdgeProperty {
    void * somePointer;
    double payload;
    EdgeProperty(double payload) : somePointer(0x0), payload(payload) {}
  };

  static void run() {
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, VertexProperty , EdgeProperty> Graph;
    typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef boost::graph_traits<Graph>::edge_descriptor Edge;
    typedef boost::graph_traits<Graph>::edge_iterator EdgeIterator;
    typedef boost::property_map<Graph, double EdgeProperty::*>::type PayloadMap;

    Graph graph;
    Vertex v1 = boost::add_vertex(VertexProperty(), graph);
    Vertex v2 = boost::add_vertex(VertexProperty(), graph);
    Vertex v3 = boost::add_vertex(VertexProperty(), graph);
    Vertex v4 = boost::add_vertex(VertexProperty(), graph);

    boost::add_edge(v1, v2, EdgeProperty(1.2), graph);
    boost::add_edge(v2, v3, EdgeProperty(2.3), graph);
    boost::add_edge(v3, v4, EdgeProperty(3.4), graph);
    boost::add_edge(v4, v1, EdgeProperty(4.1), graph);

    EdgeIterator eit, eend;
    PayloadMap map(get(&EdgeProperty::payload, graph));
    for(boost::tie(eit, eend) = boost::edges(graph); eit != eend; ++eit) {
      std::cerr << map[*eit] << std::endl;
    }
  }
};
#endif

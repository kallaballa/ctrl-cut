#ifndef INDICES_H_
#define INDICES_H_
#include <string>
#include <map>
#include <set>
#include <boost/geometry/extensions/index/rtree/rtree.hpp>

#include "Geometry.h"

using std::set;
using std::map;
using std::pair;
using std::string;
using boost::geometry::index::rtree;

struct compare_edge_vertices
{
  template<typename Edge>
  inline bool operator()(Edge const& left, Edge const& right) const {
    return left.m_source < right.m_source ||
        ((left.m_source == right.m_source) &&  (left.m_target < right.m_target));
  }
};

template<typename Graph>
class Index {
private:
  Index() {}
public:
  typedef typename boost::graph_traits<Graph>
    ::vertex_descriptor Vertex;

  typedef typename boost::graph_traits<Graph>
    ::edge_descriptor Edge;

  typedef map<Point, Vertex, boost::geometry::less<Point> > Points;
  typedef map<Linestring*, Box*> Linestrings;
  typedef rtree<Box, Linestring*> Rtree;

  static typename boost::graph_traits<Graph>::edges_size_type edge_count;

  static Points& points;

  virtual ~Index() {}
};

template<typename Graph>
typename boost::graph_traits<Graph>::edges_size_type Index<Graph>::edge_count = 0;

template<typename Graph>
typename Index<Graph>::Points& Index<Graph>::points = (* new Index<Graph>::Points());

#endif /* INDICES_H_ */

/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "util/Logger.hpp"
#include "cut/Cut.hpp"
#include "cut/geom/PointTree.hpp"
#include "cut/graph/Algorithms.hpp"
#include "cut/graph/PathGraph.hpp"
#include "cut/geom/algorithms/Append.hpp"
#include <boost/foreach.hpp>
#include <cut/operations/NearestPathSorting.hpp>

void nearest_path_sorting(Route& src, Route& sink) {
  using namespace std;
  PathGraph pathGraph;

  for(const Path& path : src) {
    pathGraph.addPath(path);
  }

  multimap<Point,PathGraph::Vertex> point_to_vertices;
  multimap<Point,PathGraph::Vertex>::iterator it_map;

  BOOST_FOREACH(const PathGraph::Vertex& v, boost::vertices(pathGraph)) {
    point_to_vertices.insert(make_pair(pathGraph[v], v));
  }

  PointTree pointTree;

  for(const Path& path : src) {
    pointTree.insert(path.front());
    pointTree.insert(path.back());
  };

  Point source(0,0);
  Point nearest;
  PathGraph::Vertex v;
  PathGraph::Edge e;

  while(!point_to_vertices.empty()) {
    PointTree::iterator it_tree = pointTree.find_nearest_not_self(source).first;
    if(it_tree == pointTree.end())
      break;

    nearest = *it_tree;

    it_map = point_to_vertices.find(nearest);
    if(it_map != point_to_vertices.end()) {
      v = (*it_map).second;
      if(boost::degree(v,pathGraph) > 0) {
        e = *(boost::out_edges(v,pathGraph).first);
        Path& path = pathGraph[e];
        if(nearest != path.front())
          boost::geometry::reverse(path);

        assert(nearest == path.front());

        append(sink,path);
        remove_edge(e,pathGraph);
        source = path.back();
      }
      point_to_vertices.erase(it_map);
      boost::remove_vertex(v,pathGraph);
    } else {
      pointTree.erase(it_tree);
    }
    std::cerr << std::endl;

  }
}


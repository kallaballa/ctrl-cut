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

#include "util/Logger.hpp"
#include "cut/Cut.hpp"
#include "cut/geom/PointTree.hpp"
#include "cut/graph/Algorithms.hpp"
#include "cut/graph/PathGraph.hpp"
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


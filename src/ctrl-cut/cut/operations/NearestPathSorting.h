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

#ifndef NEARESTPATHSORTING_H_
#define NEARESTPATHSORTING_H_

#include "util/Logger.hpp"
#include "cut/Cut.hpp"
#include "cut/geom/PointTree.hpp"
#include "cut/graph/Algorithms.hpp"
#include "cut/graph/PathGraph.hpp"
#include <boost/foreach.hpp>

template<typename TmultiPointRange>
void nearest_path_sorting(Route& src, TmultiPointRange& sink) {
  using namespace std;
  PathGraph pathGraph;

  BOOST_FOREACH(const Path& path, src) {
    pathGraph.add(path);
  }

  multimap<Point,PathGraph::Vertex> point_to_vertices;
  multimap<Point,PathGraph::Vertex>::iterator it_map;

  BOOST_FOREACH(const PathGraph::Vertex& v, boost::vertices(pathGraph)) {
    point_to_vertices.insert(make_pair(pathGraph[v], v));
  }

  PointTree pointTree;

  BOOST_FOREACH(const Path& path, src) {
    pointTree.insert(path.front());
    pointTree.insert(path.back());
  };

  Point source(0,0);
  Point nearest;
  PathGraph::Vertex v;
  PathGraph::Edge e;

  while(!point_to_vertices.empty()) {
    PointTree::iterator it_tree = pointTree.find_nearest(source).first;
    nearest = *it_tree;
    it_map = point_to_vertices.find(nearest);
    if(it_map != point_to_vertices.end()) {
      v = (*it_map).second;
      if(boost::degree(v,pathGraph) > 0) {
        e = *(boost::out_edges(v,pathGraph).first);
        append(sink,pathGraph[e]);
        remove_edge(e,pathGraph);
      }
      point_to_vertices.erase(it_map);
      boost::remove_vertex(v,pathGraph);
    } else {
      pointTree.erase(it_tree);
    }

    source = nearest;
  }
}


#endif /* NEARESTPATHSORTING_H_ */

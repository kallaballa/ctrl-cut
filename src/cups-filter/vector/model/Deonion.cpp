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
#include "Deonion.h"
#include "util/Logger.h"
#include "vector/graph/Traverse.h"

void walkTheEdge(SegmentTree& segTree, FreeGeometryView<SegmentGraph>& graph, SegmentString& skin, const SegmentGraph::Edge lastEdge, const SegmentGraph::Vertex outV)
{
  SegmentGraph::Edge nextEdge;
  const Segment* lastSegment = graph[lastEdge].segment;
  const Point* lastPoint = graph[outV].point;
  float lastSlope = lastSegment->getSlope();

  bool owned = graph[lastEdge].owner != NULL;
  if(!owned && skin.addSegment(*lastSegment)) {
    graph[lastEdge].owner = &skin;
    segTree.remove(*lastSegment);
  } else {
    assert(false);
  }

  float max_slope = 0;
  float candidateSlope;
  bool found = false;

  boost::graph_traits<FreeGeometryView<SegmentGraph> >::out_edge_iterator eit, eend;

  for(boost::tie(eit, eend) = boost::out_edges(outV, graph); eit != eend; ++eit) {
    const SegmentGraph::Edge candidate = *eit;
    const Segment* segment = graph[candidate].segment;

    //skip identical segment
    if(segment == lastSegment)
      continue;

    if((*segment)[0] != (*lastPoint))
      candidateSlope = segment->getSlope(true);
    else
      candidateSlope = segment->getSlope();

    std::cerr << *segment << std::endl;
    std::cerr << candidateSlope << std::endl;

    //save the edge with the least clockwise angle (= least slop_diff)
    if (candidateSlope > max_slope && candidateSlope < lastSlope) {
      max_slope = candidateSlope;
      nextEdge = candidate;
      found = true;
    }
  }

  if (found) {
    walkTheEdge(segTree, graph, skin, nextEdge, get_opposite(graph, nextEdge, outV));
  }

  //polyline fully consumed
}

void traverse_onion(StringList& skins, SegmentList::iterator first, SegmentList::iterator last)
{
  LOG_INFO_STR("Deonion");
  SegmentGraph graph;
  create_planar_graph(graph, first, last);
  SegmentTree& segTree = SegmentTree::build(first,last);
  const SegmentGraph::PointMap& point2Vertex = graph.getPointMap();
  FreeGeometryView<SegmentGraph> freeGeometryView(graph);
  boost::graph_traits<FreeGeometryView<SegmentGraph> >::out_edge_iterator eit, eend;

  while (!segTree.empty()) {
    SegmentGraph::Vertex startVertex;
    const Point* startPoint = NULL;

    for(SegmentGraph::PointMap::const_iterator it = point2Vertex.begin(); it != point2Vertex.end(); ++it) {
      startVertex = (*it).second;
      boost::tie(eit, eend) = boost::out_edges(startVertex, freeGeometryView);

      if(eit != eend) {
        // found vertex of a free edge
        startPoint = freeGeometryView[startVertex].point;
        break;
      }
    }

    // no more free edges
    if(startPoint == NULL)
      break;

    SegmentString* skin  = new SegmentString();
    const SegmentGraph::Edge steapest = find_steapest(freeGeometryView, eit, eend);
    const SegmentGraph::Vertex outVertex = get_opposite(freeGeometryView,steapest, startVertex);
    const Point& outPoint = *freeGeometryView[outVertex].point;
    if(outPoint.y < (*startPoint).y || (outPoint.y == (*startPoint).y && outPoint.x > (*startPoint).x))
      walkTheEdge(segTree, freeGeometryView, *skin, steapest, outVertex);
    else
      walkTheEdge(segTree, freeGeometryView, *skin, steapest, startVertex);

    skins.push_back(skin);
  }

  reverse(skins.begin(), skins.end());
}

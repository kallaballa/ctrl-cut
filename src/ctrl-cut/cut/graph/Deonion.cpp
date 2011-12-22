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
#include "cut/graph/Traverse.h"

void walkTheEdge(SegmentTree& segTree, FreeGeometryView<SegmentGraph>& graph, SegmentString& skin, const SegmentGraph::Edge lastEdge, const SegmentGraph::Vertex outV)
{
  SegmentGraph::Edge nextEdge;
  const Segment* lastSegment = graph[lastEdge].segment;
  const Point* lastPoint = graph[outV].point;
  float lastSlope;

  if((*lastPoint) == (*lastSegment)[0])
    lastSlope = lastSegment->getSlope();
  else if((*lastPoint) == (*lastSegment)[1])
    lastSlope = lastSegment->getSlope(true);
  else
    assert(false);

  bool owned = graph[lastEdge].owner != NULL;

  if(!owned && skin.add(lastSegment)) {
    graph[lastEdge].owner = &skin;
    segTree.remove(*lastSegment);
  } else {
    return;
  }

  float minSlopeDiff = (CC_PI * 2);
  double slopeDiff;
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

    slopeDiff = lastSlope - candidateSlope;

    if(slopeDiff < 0.01)
      slopeDiff += CC_PI * 2;

    /*std::cerr << *segment << std::endl;
    std::cerr << candidateSlope << std::endl;*/

    //save the edge with the least clockwise angle (= least slop_diff)
    if (slopeDiff < minSlopeDiff) {
      minSlopeDiff = slopeDiff;
      nextEdge = candidate;
      found = true;
    }
  }

  if (found) {
    walkTheEdge(segTree, graph, skin, nextEdge, get_opposite(graph, nextEdge, outV));
  }

  //polyline fully consumed
}

SegmentGraph::Vertex* pop_mostleft_free_vertex(SegmentGraph::PointMap& point2Vertex, const FreeGeometryView<SegmentGraph> freeGeometryView) {
  boost::graph_traits<FreeGeometryView<SegmentGraph> >::out_edge_iterator eit, eend;
  for(SegmentGraph::PointMap::iterator it = point2Vertex.begin(); it != point2Vertex.end(); ++it) {
    SegmentGraph::Vertex& candidate = (*it).second;
    point2Vertex.erase(it);

    boost::tie(eit, eend) = boost::out_edges(candidate, freeGeometryView);
    if(eit != eend) {
      // found vertex of a free edge
      return &candidate;
    }
  }
  return NULL;
}

void traverse_onion(StringList& skins, SegmentList::iterator first, SegmentList::iterator last)
{
  LOG_INFO_STR("Deonion");
  SegmentGraph graph;
  create_planar_graph(graph, first, last);
  SegmentTree& segTree = SegmentTree::build(first,last);
  FreeGeometryView<SegmentGraph> freeGeometryView(graph);
  SegmentGraph::PointMap point2Vertex = graph.getPointMap();
  boost::graph_traits<FreeGeometryView<SegmentGraph> >::out_edge_iterator eit, eend;

  while (!segTree.empty()) {
    SegmentGraph::Vertex* startVertex = pop_mostleft_free_vertex(point2Vertex, freeGeometryView);
    // no more vertices with free edges
    if(startVertex == NULL)
      break;

    const Point& startPoint = *freeGeometryView[*startVertex].point;

    SegmentString* skin  = new SegmentString();

    boost::tie(eit, eend) = boost::out_edges(*startVertex, freeGeometryView);
    const SegmentGraph::Edge steapest = find_steapest(freeGeometryView, eit, eend);
    const SegmentGraph::Vertex outVertex = get_opposite(freeGeometryView,steapest, *startVertex);
    const Point& outPoint = *freeGeometryView[outVertex].point;

    if(outPoint.y < startPoint.y || (outPoint.y == startPoint.y && outPoint.x > startPoint.x))
      walkTheEdge(segTree, freeGeometryView, *skin, steapest, outVertex);
    else
      walkTheEdge(segTree, freeGeometryView, *skin, steapest, *startVertex);

    skins.push_back(skin);
  }

  reverse(skins.begin(), skins.end());
}

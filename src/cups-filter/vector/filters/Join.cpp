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


#include "Join.h"
#include "util/Logger.h"
#include "vector/Traverse.h"

struct join_strings_visitor: public planar_face_traversal_visitor {
  CutModel& model;
  PointGraph& graph;
  SegmentString* current;

  join_strings_visitor(CutModel& model, PointGraph& graph) :
    model(model), graph(graph), current(NULL) {
  }

  void begin_face() {
//    std::cerr << "begin face " << std::endl;
  }

  void end_face() {
    if (current != NULL)
      model.add(*current);

    current = NULL;
  }

  void next_edge(PointGraph::Edge e) {
    const Segment* seg = graph.getSegment(e);

    if (graph.getSegmentString(e) == NULL) {
      if (current == NULL || !current->addSegment(*seg)) {
        current = new SegmentString();
        current->addSegment(*seg);
      }

      graph.setSegmentString(e, *current);
    }
  }
};
/*
 * Split segments at intersection points.
 */
void Join::filter(CutModel& model) {
  LOG_INFO_STR("Join");
  PointGraph& graph = PointGraph::createPlanarGraph(model.beginSegments(), model.endSegments());
  join_strings_visitor vis = *new join_strings_visitor(model, graph);
  traverse_planar_faces(graph, vis);
}

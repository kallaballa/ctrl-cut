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

#include "util/Logger.h"
#include "Explode.h"
#include "cut/graph/Traverse.h"
#include "cut/geom/SegmentTree.h"
#include <list>

void findWithinRange(std::list<SegmentNode>& in_range, const SegmentTree& segTree, const Segment& seg) {
  if(!in_range.empty())
    in_range.clear();
  segTree.findWithinRange(seg, in_range);
}

/*
 * Split segments at intersection points.
 */
void explode_segments(Route& route, SegmentList::iterator, SegmentList::iterator) {
  LOG_INFO_STR("Explode");
  LOG_DEBUG_MSG("Segments before", route.size());
  SegmentTree segTree;
  SegmentTree::build(segTree, route.segmentView().begin(), route.segmentView().end());

  std::list<SegmentNode> in_range;
  Point intersection;

  SegmentList::iterator it_s = route.segmentView().begin();

  while (it_s != route.segmentView().end()) {
    Segment& pick = (*it_s);

    bool remove_pick = false;
    bool remove_candidate = false;

    findWithinRange(in_range, segTree, pick);
    std::list<SegmentNode>::iterator it_o = in_range.begin();

    while(it_o != in_range.end()) {
      remove_candidate = false;
      Segment& candidate = (*it_o).owner;

      if(pick == candidate) {
        //FIXME duplicate line leak;
        ++it_o;
        continue;
      }
      intersection_result is_res = intersects(pick, candidate, intersection);

      // check if pick does intersect candidate
      if (is_res == ALIGN_INTERSECT) {
       if(pick[0] != intersection && pick[1] != intersection) {
         remove_pick = true;
         route.append(pick[0], intersection);
         route.append(pick[1], intersection);
        }

        if(candidate[0] != intersection && candidate[1] != intersection) {
          remove_candidate = true;
          route.append(candidate[0], intersection);
          route.append(candidate[1], intersection);
        }
      } else if(is_res == ALIGN_COINCIDENCE) {
        bool firstMatches = pick[0] == candidate[0] || pick[0] == candidate[1];
        bool secondMatches = pick[1] == candidate[0] || pick[1] == candidate[1];
        if(firstMatches || secondMatches) {
          if(firstMatches && secondMatches) {
            //eliminated duplicate segments
            remove_pick = true;
          }
        } else {
          //coincidental but neither tip connected nor identical
          Point pick_min;
          Point pick_max;
          Point candidate_min;
          Point candidate_max;

          if(pick.first < pick.second) {
            pick_min = pick.first;
            pick_max = pick.second;
          } else {
            pick_min = pick.second;
            pick_max = pick.first;
          }

          if(candidate.first < candidate.second) {
            candidate_min = candidate.first;
            candidate_max = candidate.second;
          } else {
            candidate_min = candidate.second;
            candidate_max = candidate.first;
          }

          // FIXME which gets which settings?
          if(candidate_min < pick_max) {
            if(pick_min < candidate_min) {
              remove_pick = true;
              remove_candidate = true;
              if(pick_max < candidate_max) {
                route.append(pick_min, candidate_min);
                route.append(candidate_min, pick_max);
                route.append(pick_max, candidate_max);
              } else {
                route.append(pick_min, candidate_min);
                route.append(candidate_min, candidate_max);
                route.append(candidate_max, pick_max);
              }
            } else if(pick_min < candidate_max) {
              remove_pick = true;
              remove_candidate = true;
              route.append(candidate_min, pick_min);
              route.append(pick_min, candidate_max);
              route.append(candidate_max, pick_max);
            }
          }
        }
      }

      if(remove_candidate) {
        segTree.remove((*it_o).owner);

        //FIXME find out why maintaining the in_range list incrementally produces invalid iterators
        if(!remove_pick) {
          //don't search again if the loop is going to break*/
          findWithinRange(in_range,segTree,pick);
          it_o = in_range.begin();
        }
      } else
        ++it_o;

      if(remove_pick) {
        segTree.remove(pick);
        break;
      }
    };

    if(!remove_pick)
      ++it_s;
  }
  LOG_DEBUG_MSG("Segments after", route.size());
}

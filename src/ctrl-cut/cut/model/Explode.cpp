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
#include <list>

void findWithinRange(SegmentTree& segTree, CutModel::iterator it_s, std::list<SegmentNode>& in_range) {
  if(!in_range.empty())
    in_range.clear();
  segTree.findWithinRange(it_s, in_range);
}

/*
 * Split segments at intersection points.
 */
void explode_segments(CutModel& model) {
  LOG_INFO_STR("Explode");
  LOG_DEBUG_MSG("Segments before", model.size());
  SegmentTree& segTree = SegmentTree::build(model.begin(), model.end());

  std::list<SegmentNode> in_range;
  Point intersection;
  CutModel::iterator it_s = model.begin();

  while (it_s != model.end()) {
    Segment& pick = *(*it_s);

    bool remove_pick = false;
    bool remove_candidate = false;

    findWithinRange(segTree, it_s, in_range);
    std::list<SegmentNode>::iterator it_o = in_range.begin();

    while(it_o != in_range.end()) {
      remove_candidate = false;
      Segment& candidate = **(*it_o).getIterator();

      if(&pick == &candidate) {
        ++it_o;
        continue;
      }
      intersection_result is_res = intersects(pick, candidate, intersection);

      // check if pick does intersect candidate
      if (is_res == ALIGN_INTERSECT) {
       if(pick[0] != intersection && pick[1] != intersection) {
         remove_pick = true;
         model.createSegment(pick[0], intersection, pick.settings);
         model.createSegment(pick[1], intersection, pick.settings);
        }

        if(candidate[0] != intersection && candidate[1] != intersection) {
          remove_candidate = true;
          model.createSegment(candidate[0], intersection, candidate.settings);
          model.createSegment(candidate[1], intersection, candidate.settings);
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
          Point* pick_min;
          Point* pick_max;
          Point* candidate_min;
          Point* candidate_max;

          if(pick.first < pick.second) {
            pick_min = &pick.first;
            pick_max = &pick.second;
          } else {
            pick_min = &pick.second;
            pick_max = &pick.first;
          }

          if(candidate.first < candidate.second) {
            candidate_min = &candidate.first;
            candidate_max = &candidate.second;
          } else {
            candidate_min = &candidate.second;
            candidate_max = &candidate.first;
          }

          // FIXME which gets which settings?
          if((*candidate_min) < (*pick_max)) {
            if((*pick_min) < (*candidate_min)) {
              remove_pick = true;
              remove_candidate = true;
              if((*pick_max) < (*candidate_max)) {
                model.createSegment(*pick_min, *candidate_min, pick.settings);
                model.createSegment(*candidate_min, *pick_max, pick.settings);
                model.createSegment(*pick_max, *candidate_max, candidate.settings);
              } else {
                model.createSegment(*pick_min, *candidate_min, pick.settings);
                model.createSegment(*candidate_min, *candidate_max, candidate.settings);
                model.createSegment(*candidate_max, *pick_max, pick.settings);
              }
            } else if((*pick_min) < (*candidate_max)) {
              remove_pick = true;
              remove_candidate = true;
              model.createSegment(*candidate_min, *pick_min, candidate.settings);
              model.createSegment(*pick_min, *candidate_max, candidate.settings);
              model.createSegment(*candidate_max, *pick_max, pick.settings);
            }
          }
        }
      }

      if(remove_candidate) {
        segTree.remove((*it_o).getIterator());
        model.erase((*it_o).getIterator());

        //FIXME find out why maintaining the in_range list incrementally produces invalid iterators
        if(!remove_pick) {
          //don't search again if the loop is going to break*/
          findWithinRange(segTree,it_s, in_range);
          it_o = in_range.begin();
        }
      } else
        ++it_o;

      if(remove_pick) {
        segTree.remove(it_s);
        it_s = model.erase(it_s);
        break;
      }
    };

    if(!remove_pick)
      ++it_s;
  }
  LOG_DEBUG_MSG("Segments after", model.size());
  delete &segTree;
}

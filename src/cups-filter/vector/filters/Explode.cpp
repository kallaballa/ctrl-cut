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

#include <deque>
#include "util/Logger.h"
#include "Explode.h"

using std::deque;

/*
 * Split segments at intersection points.
 */
void Explode::filter(CutModel& cut) {
  LOG_INFO_STR("Explode");
  vector<SegmentNode> in_range;
  Point intersection;

  for (CutModel::SegmentIter it_s = cut.beginSegments(); it_s != cut.endSegments(); ++it_s) {
    if(it_s == cut.endSegments())
      continue;
    const Segment& pick = *(*it_s);

    if(!in_range.empty())
      in_range.clear();
    cut.findWithinRange(it_s, in_range);

    for (vector<SegmentNode>::iterator it_o = in_range.begin(); it_o != in_range.end(); ++it_o) {
      const Segment& candidate = *(*((*it_o).owner));
      if(&pick == &candidate)
        continue;

      intersection_result is_res = intersects(pick, candidate, intersection);

      // check if pick does intersect candidate
      if (is_res == ALIGN_INTERSECT) {
        // if pick doesnt tip intersect remove it and split it in two
        if (!(pick[0] == intersection) && !(pick[1] == intersection)) {
         it_s = cut.remove(it_s);
         cut.createSegment(pick[0], intersection, pick.settings);
         cut.createSegment(pick[1], intersection, pick.settings);
        }

        // if candidate doesnt tip intersect remove it and split it in two
        if (!(candidate[0] == intersection) && !(candidate[1] == intersection)) {
         cut.remove((*it_o).owner);
         cut.createSegment(candidate[0], intersection, candidate.settings);
         cut.createSegment(candidate[1], intersection, candidate.settings);
        }
      } else if(is_res == ALIGN_PARALLEL) {
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
            it_s = cut.remove(it_s);
            cut.remove((*it_o).owner);

            if(pick_max < candidate_max) {
              cut.createSegment(pick_min, candidate_min, pick.settings);
              cut.createSegment(candidate_min, pick_max, pick.settings);
              cut.createSegment(pick_max, candidate_max, candidate.settings);
            } else {
              cut.createSegment(pick_min, candidate_min, pick.settings);
              cut.createSegment(candidate_min, candidate_max, candidate.settings);
              cut.createSegment(candidate_max, pick_max, pick.settings);
            }
          } else if(pick_min < candidate_max) {
            it_s = cut.remove(it_s);
            cut.remove((*it_o).owner);
            cut.createSegment(candidate_min, pick_min, candidate.settings);
            cut.createSegment(pick_min, candidate_max, candidate.settings);
            cut.createSegment(candidate_max, pick_max, pick.settings);
          }
        }
      }
    }
  }
}

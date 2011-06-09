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
  vector<SpatialItem> in_range;

  for (CutModel::iterator it_s = cut.begin(); it_s != cut.end(); ++it_s) {
    if(it_s == cut.end())
      continue;
    Segment* pick = *it_s;
    if(!in_range.empty())
      in_range.clear();
    cut.findWithinRange(it_s, in_range);

    for (vector<SpatialItem>::iterator it_o = in_range.begin(); it_o != in_range.end(); ++it_o) {
      Segment* candidate = *((*it_o).owner);
      if(pick == candidate)
        continue;

      Point* intersec;
      // check if pick does intersect candidate
      if ((intersec = intersects(*pick, *candidate)) != NULL) {

        // if pick doesnt tip intersect remove it and split it in two
        if (!((*pick)[0] == *intersec) && !((*pick)[1] == *intersec)) {
         it_s = cut.removeSegment(it_s);
         cut.createSegment((*pick)[0], *intersec, pick->settings);
         cut.createSegment((*pick)[1], *intersec, pick->settings);
        }

        // if candidate doesnt tip intersect remove it and split it in two
        if (!((*candidate)[0] == *intersec) && !((*candidate)[1] == *intersec)) {
         cut.removeSegment((*it_o).owner);
         cut.createSegment((*candidate)[0], *intersec, candidate->settings);
         cut.createSegment((*candidate)[1], *intersec, candidate->settings);
        }
      }
    }
  }
}

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

#ifndef EXPLODE_H_
#define EXPLODE_H_


#include "util/Logger.hpp"
#include "cut/geom/SegmentTree.hpp"
#include "cut/geom/Views.hpp"
#include "svg/SvgPlot.hpp"

//#define CC_EXPLODE_DEBUG 1

void debug(const Segment& seg);
void debug(const Segment& pick, const Segment& candidate);
void debug(const Segment& pick, const Segment& candidate, const Point& intersection);
void debugEmitted(const Segment& seg);
void debugTree(IndexedSegmentTree& tree);
void printSegment(const Segment& seg, size_t indent);

/*
 * Split segments at intersection points.
 */
template<
  typename TmultiPointRange
>
class Explode {
private:
  TmultiPointRange& src;
  TmultiPointRange& sink;

  IndexedSegmentTree tree;

  typedef IndexedSegmentTree::iterator TreeIter;
  typedef IndexedSegmentTree::Result Result;
  typedef IndexedSegmentTree::Result::const_iterator ResultIter;
public:
  Explode(TmultiPointRange& src, TmultiPointRange& sink) :
    src(src), sink(sink){
  }

  Explode(const Explode& other) :
    src(other.src), sink(other.sink)
  {}

  void push_back(const Point& first, const Point& second) {
    this->push_back(Segment(first,second));
  }

  void push_back(const Segment& seg) {
    debugEmitted(seg);

    if(seg[0] != seg[1]) //ignore zero length segments produced by intersections
      this->tree.push_back(seg);
  }

  TreeIter erase(TreeIter& it) {
    return this->tree.erase(it);
  }

  void intersect(const Point& intersection, TreeIter& it_pick, TreeIter& it_candidate) {
    const Segment& pick = *it_pick;
    const Segment& candidate = *it_candidate;

    if (candidate[0] != intersection && candidate[1] != intersection) {
      this->push_back(candidate[0], intersection);
      this->push_back(candidate[1], intersection);
      it_candidate = this->erase(it_candidate);
    }

    if (pick[0] != intersection && pick[1] != intersection) {
      this->push_back(pick[0], intersection);
      this->push_back(pick[1], intersection);
      it_pick = this->erase(it_pick);
    }
    debugTree(tree);
  }

  void intersectCoincidence (TreeIter& it_pick, TreeIter& it_candidate) {
    const Segment& pick = *it_pick;
    const Segment& candidate = *it_candidate;

    //coincidental but neither tip connected nor identical
    Point pick_min;
    Point pick_max;
    Point candidate_min;
    Point candidate_max;

    if (pick.first < pick.second) {
      pick_min = pick.first;
      pick_max = pick.second;
    } else {
      pick_min = pick.second;
      pick_max = pick.first;
    }

    if (candidate.first < candidate.second) {
      candidate_min = candidate.first;
      candidate_max = candidate.second;
    } else {
      candidate_min = candidate.second;
      candidate_max = candidate.first;
    }

    if (candidate_min < pick_max) {
      if (pick_min < candidate_min) {
        if (pick_max < candidate_max) {
          this->push_back(pick_min, candidate_min);
          this->push_back(candidate_min, pick_max);
          this->push_back(pick_max, candidate_max);
          it_pick = this->erase(it_pick);
          it_candidate = this->erase(it_candidate);
        } else {
          this->push_back(pick_min, candidate_min);
          this->push_back(candidate_min, candidate_max);
          this->push_back(candidate_max, pick_max);
          it_candidate = this->erase(it_candidate);
        }
      } else if (pick_min < candidate_max) {
        this->push_back(candidate_min, pick_min);
        this->push_back(pick_min, candidate_max);
        this->push_back(candidate_max, pick_max);
        it_pick = this->erase(it_pick);
        it_candidate = this->erase(it_candidate);
      }
    }
    debugTree(tree);
  }

  void operator()() {
    LOG_INFO_STR("Explode");

    Point intersection;

    for(const SegmentPtr seg : segments(src)) {
      tree.push_back(*seg.get());
    }

    for(size_t n=0; n < 3; ++n) {
      for (TreeIter it_pick = tree.begin(); it_pick != tree.end(); ++it_pick) {
        const Segment& pick = (*it_pick);
        debug(pick);

        if(pick[0] == pick[1])
          continue;

       const Result& in_range = tree.findWithinRange(it_pick);

       for(TreeIter it_candidate : in_range) {
         if(it_pick == it_candidate)
           continue;
          const Segment& candidate = *it_candidate;

          debug(pick,candidate);
          if (pick != candidate && candidate[0] != candidate[1]) {
            if (pick[0] != candidate[0] &&
                pick[0] != candidate[1] &&
                pick[1] != candidate[0] &&
                pick[1] != candidate[1]) {
              intersection_result is_res = intersects(pick, candidate, intersection);
              // check if pick does intersect candidate
              if (is_res == ALIGN_INTERSECT) {
                debug(pick,candidate, intersection);
                intersect(intersection, it_pick, it_candidate);
                break;
              } else if (is_res == ALIGN_COINCIDENCE) {
                debug(pick,candidate, intersection);
                intersectCoincidence(it_pick, it_candidate);
                break;
              }
            }
          }
        }
      }
    }

    debugTree(this->tree);
    for(TreeIter it = this->tree.begin(); it != this->tree.end(); ++it) {
      add(sink, *it);
    }
  }
};


template<
  typename TmultiPointRange
>
void explode(TmultiPointRange& src, TmultiPointRange& sink) {
  Explode<TmultiPointRange> exploder(src,sink);
  exploder();
}

#endif

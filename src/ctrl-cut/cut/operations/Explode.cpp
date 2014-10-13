/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Explode.hpp"
#include "util/Logger.hpp"
#include "cut/geom/SegmentTree.hpp"
#include "cut/geom/algorithms/Add.hpp"
#include "cut/geom/algorithms/Append.hpp"
#include "math.h"

//#define CC_EXPLODE_DEBUG 1

#ifdef CC_EXPLODE_DEBUG

void printSegment(const Segment& seg, size_t indent) {
  for(;indent > 0; --indent)
    std::cerr << "\t";

  std::cerr << seg.first.x << ","
      << seg.first.y << " "
      << seg.second.x << ","
      << seg.second.y << std::endl;
}
void debug(const Segment& pick) {
  if(Segment(Point(1005,12387), Point(1005,13798)) == pick)
    std::cerr << "";
  printSegment(pick, 0);
}

void debug(const Segment& pick, const Segment& candidate) {
  printSegment(candidate, 1);
}

void debug(const Segment& pick, const Segment& candidate, const Point& intersection) {
  std::cerr << "\t\t" << intersection.x << "," << intersection.y << std::endl;
}

void debugEmitted(const Segment& s0) {
  printSegment(s0,3);
}

void debugTree(IndexedSegmentTree& tree) {
  std::cerr << std::endl;
  std::cerr << std::endl;
  for(IndexedSegmentTree::iterator it = tree.begin(); it != tree.end(); ++it) {
    debug(*it);
  }
  std::cerr << std::endl;
  std::cerr << std::endl;
}

void checkStableSlope(const Segment& pick, const Segment& candidate,
    const Point& intersection) {
  float sp = pick.getSlope();
  float sp1;

  if (pick[0] == intersection)
    sp1 = Segment(pick[1], intersection).getSlope();
  else
    sp1 = Segment(pick[0], intersection).getSlope();

  float sc = candidate.getSlope();
  float sc1;

  if (candidate[0] == intersection)
    sc1 = Segment(candidate[1], intersection).getSlope();
  else
    sc1 = Segment(candidate[0], intersection).getSlope();

  assert(abs(sp - sp1) <= 0.01 || abs(sc - sc1) <= 0.01);
}

void checkStableSlopeEmitted(const Segment& s0, const Segment& s1, const Segment& s2) {
   float ss0 = s0.getSlope();
   float ss1 = s1.getSlope();
   float ss2 = s2.getSlope();

   assert(abs(abs(ss0 - ss1) - ss2) <= 0.02);
}

#else
void printSegment(const Segment& seg, size_t indent) {}
void debug(const Segment& pick) {}
void debug(const Segment& pick, const Segment& candidate) {}
void debug(const Segment& pick, const Segment& candidate, const Point& intersection) {}
void debugEmitted(const Segment& s0) {}
void debugTree(IndexedSegmentTree& tree) {}
void checkStableSlope(const Segment& pick, const Segment& candidate, const Point& intersection) {}
void checkStableSlopeEmitted(const Segment& s0, const Segment& s1, const Segment& s2) {}
#endif

/*
 * Split segments at intersection points.
 */
class Explode {
private:
  Route& src;
  Route& sink;

  IndexedSegmentTree tree;

  typedef IndexedSegmentTree::iterator TreeIter;
  typedef IndexedSegmentTree::Result Result;
  typedef IndexedSegmentTree::Result::const_iterator ResultIter;
public:
  Explode(Route& src, Route& sink) :
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

void explode(Route& src, Route& sink) {
  Explode exploder(src,sink);
  exploder();
}


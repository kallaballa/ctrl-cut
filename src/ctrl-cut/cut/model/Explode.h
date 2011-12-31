#ifndef EXPLODE_H_
#define EXPLODE_H_

#include "util/Logger.h"
#include "Explode.h"
#include "cut/graph/Traverse.h"
#include "cut/geom/SegmentTree.h"
#include <list>

/*
 * Split segments at intersection points.
 */
template<
  typename TsegmentInputIterator,
  typename TsegmentOutputIterator
>
class Explode {
private:
  TsegmentInputIterator begin;
  TsegmentInputIterator end;
  TsegmentOutputIterator sink;

  IndexedSegmentTree tree;

  typedef IndexedSegmentTree::iterator TreeIter;
  typedef IndexedSegmentTree::Result Result;
  typedef IndexedSegmentTree::Result::const_iterator ResultIter;

public:
  Explode(TsegmentInputIterator begin, TsegmentInputIterator end, TsegmentOutputIterator sink) : begin(begin), end(end), sink(sink) {
    BOOST_CONCEPT_ASSERT((SegmentInputIterator<TsegmentInputIterator>));
    BOOST_CONCEPT_ASSERT((SegmentOutputIterator<TsegmentOutputIterator>));
    for (TsegmentInputIterator it_copy = begin; it_copy != end; ++it_copy) {
      tree.push_back(*it_copy);
    }
  }

  Explode(const Explode& other) :
    begin(other.begin), end(other.end), sink(other.sink)
  {}

  void push_back(const Point& first, const Point& second) {
    this->push_back(Segment(first,second));
  }

  void push_back(const Segment& seg) {
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
  }

  void intersectCoincidence (const Point& intersection, TreeIter& it_pick, TreeIter& it_candidate) {
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

    // FIXME which gets which settings?
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
  }

  void operator()() {
    LOG_INFO_STR("Explode");

    Point intersection;
    tree.build(begin, end);


    for (TreeIter it_pick = tree.begin(); it_pick != tree.end(); ++it_pick) {
      std::cerr << *it_pick << std::endl;

      const Segment& pick = (*it_pick);
      assert(pick[0] != pick[1]);

     const Result& in_range = tree.findWithinRange(it_pick);

      for (ResultIter it_result = in_range.begin(); it_result != in_range.end(); ++it_result) {
        TreeIter it_candidate = *it_result;
        const Segment& candidate = *it_candidate;
        assert(candidate[0] != candidate[1]);

        if (pick == candidate) {
          continue;
        }

        intersection_result is_res = intersects(pick, candidate, intersection);

        // check if pick does intersect candidate
        if (is_res == ALIGN_INTERSECT) {
          intersect(intersection, it_pick, it_candidate);
        } else if (is_res == ALIGN_COINCIDENCE) {
          intersectCoincidence(intersection, it_pick, it_candidate);
        }
      }
    }
    std::copy(tree.begin(), tree.end(), sink);
  }
};

template<
  typename TsegmentOutputIterator,
  typename TsegmentInputIterator
>
void explode(TsegmentInputIterator begin, TsegmentInputIterator end, TsegmentOutputIterator sink) {
  Explode<TsegmentInputIterator, TsegmentOutputIterator> exploder(begin,end,sink);
  exploder();
}
#endif

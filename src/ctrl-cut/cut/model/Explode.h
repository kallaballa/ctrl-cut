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
template<typename TsegmentOutputIterator, typename TsegmentInputIterator>
void explode_segments(TsegmentOutputIterator sink, TsegmentInputIterator begin, TsegmentInputIterator end) {
  BOOST_CONCEPT_ASSERT((SegmentInputIterator<TsegmentInputIterator>));
  BOOST_CONCEPT_ASSERT((SegmentOutputIterator<TsegmentOutputIterator>));

  LOG_INFO_STR("Explode");
  SegmentTree segTree;
  build(segTree, begin, end);
  Point intersection;

  TsegmentInputIterator it_s = begin;

  for(SegmentTree::const_iterator it_pick = segTree.begin(); it_pick != segTree.end(); ++it_pick) {
    const Segment& pick = (*it_pick);
    std::list<Segment> in_range;
    segTree.findWithinRange(pick, in_range);

    bool remove_pick = false;
    bool remove_candidate = false;

    std::list<Segment>::iterator it_candidate = in_range.begin();
    while(it_candidate != in_range.end()) {
      remove_candidate = false;
      const Segment& candidate = *it_candidate;

      if(pick == candidate) {
        //FIXME duplicate line leak;

        ++it_candidate;
        continue;
      }
      intersection_result is_res = intersects(pick, candidate, intersection);

      // check if pick does intersect candidate
      if (is_res == ALIGN_INTERSECT) {
       if(pick[0] != intersection && pick[1] != intersection) {
         remove_pick = true;
         *(sink++) = Segment(pick[0], intersection);
         *(sink++) = Segment(pick[1], intersection);
        }

        if(candidate[0] != intersection && candidate[1] != intersection) {
          remove_candidate = true;
          *(sink++) = Segment(candidate[0], intersection);
          *(sink++) = Segment(candidate[1], intersection);
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
                *(sink++) = Segment(pick_min, candidate_min);
                *(sink++) = Segment(candidate_min, pick_max);
                *(sink++) = Segment(pick_max, candidate_max);
              } else {
                *(sink++) = Segment(pick_min, candidate_min);
                *(sink++) = Segment(candidate_min, candidate_max);
                *(sink++) = Segment(candidate_max, pick_max);
              }
            } else if(pick_min < candidate_max) {
              remove_pick = true;
              remove_candidate = true;
              *(sink++) = Segment(candidate_min, pick_min);
              *(sink++) = Segment(pick_min, candidate_max);
              *(sink++) = Segment(candidate_max, pick_max);
            }
          }
        }
      }

      if(remove_candidate) {
        segTree.remove((*it_candidate).owner);

        //FIXME find out why maintaining the in_range list incrementally produces invalid iterators
        if(!remove_pick) {
          //don't search again if the loop is going to break*/
          in_range.clear();
          segTree.findWithinRange(pick, in_range);
          it_candidate = in_range.begin();
        }
      } else
        ++it_candidate;

      if(remove_pick) {
        segTree.remove(pick);
        break;
      }
    };

    if(!remove_pick)
      ++it_pick;
  }
}


#endif

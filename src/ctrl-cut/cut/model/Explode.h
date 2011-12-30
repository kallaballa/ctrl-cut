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

void insert(SegmentTree& segTree, std::list<Segment>& segmentList, const Segment& seg) {
  if(seg[0] == seg[1])
    return;

  segTree.insert(seg);
  segmentList.push_back(seg);
}

template<typename TsegmentOutputIterator, typename TsegmentInputIterator>
void explode_segments(TsegmentOutputIterator sink, TsegmentInputIterator begin,
    TsegmentInputIterator end) {
  BOOST_CONCEPT_ASSERT((SegmentInputIterator<TsegmentInputIterator>));
  BOOST_CONCEPT_ASSERT((SegmentOutputIterator<TsegmentOutputIterator>));

  LOG_INFO_STR("Explode");

  Point intersection;
  SegmentTree segTree;
  build(segTree,begin,end);
  std::list<Segment> segmentList;

  for (TsegmentInputIterator it_copy = begin; it_copy != end; ++it_copy) {
    segmentList.push_back(*it_copy);
  }
  long i = 0;
  for (std::list<Segment>::iterator it_pick = segmentList.begin(); it_pick != segmentList.end(); ++it_pick) {
    const Segment& pick = (*it_pick);
    if(pick[0] == pick[1])
      continue;

    std::list<SegmentNode> in_range;
    segTree.findWithinRange(pick, in_range);

    for (std::list<SegmentNode>::iterator it_candidate = in_range.begin(); it_candidate != in_range.end(); ++it_candidate) {
      const SegmentNode& candidate = *it_candidate;
      if(candidate[0] == candidate[1])
          continue;
      if (pick == candidate) {
        continue;
      }

      intersection_result is_res = intersects(pick, candidate, intersection);

      // check if pick does intersect candidate
      if (is_res == ALIGN_INTERSECT) {
        if (candidate[0] != intersection && candidate[1] != intersection) {
          insert(segTree,segmentList,Segment(candidate[0], intersection));
          insert(segTree,segmentList,Segment(candidate[1], intersection));

          segmentList.remove(candidate);
          segTree.erase_exact(candidate);
        }

        if (pick[0] != intersection && pick[1] != intersection) {
          insert(segTree,segmentList,Segment(pick[0], intersection));
          insert(segTree,segmentList,Segment(pick[1], intersection));

          it_pick = segmentList.erase(it_pick);
          segTree.erase_exact(pick);
          break;
        }

      } else if (is_res == ALIGN_COINCIDENCE) {
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
              insert(segTree,segmentList,Segment(pick_min, candidate_min));
              insert(segTree,segmentList,Segment(candidate_min, pick_max));
              insert(segTree,segmentList,Segment(pick_max, candidate_max));

              it_pick = segmentList.erase(it_pick);
              segmentList.remove(candidate);

              segTree.erase_exact(pick);
              segTree.erase_exact(candidate);
              break;
            } else {
              insert(segTree,segmentList,Segment(pick_min, candidate_min));
              insert(segTree,segmentList,Segment(candidate_min, candidate_max));
              insert(segTree,segmentList,Segment(candidate_max, pick_max));

              segmentList.remove(candidate);
              segTree.erase_exact(candidate);
            }
          } else if (pick_min < candidate_max) {
            insert(segTree,segmentList,Segment(candidate_min, pick_min));
            insert(segTree,segmentList,Segment(pick_min, candidate_max));
            insert(segTree,segmentList,Segment(candidate_max, pick_max));

            it_pick = segmentList.erase(it_pick);
            segmentList.remove(candidate);

            segTree.erase_exact(pick);
            segTree.erase_exact(candidate);
            break;
          }

        }
      }
    }
    std::cerr << segTree.size() << "\t" << i++ << std::endl;
    if(it_pick == segmentList.end())
      break;
  }
  std::copy(segTree.begin(),segTree.end(), sink);
};

#endif

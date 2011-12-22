#include "CutView.h"

void CutView::add(const Segment& seg) {
  const Segment& clipped = CutModel::clip(seg);
  if (clipped.first == clipped.second) // ignore zero length segments
    return;
  segmentIndex.push_back(&clipped);
  increment.insert(&clipped);
}

void CutView::remove(const Segment& seg) {
  SegmentSet::iterator it_inc = increment.find(&seg);
  if(it_inc != increment.end()) {
    increment.erase(it_inc);
    CutModel::remove(seg);
  } else
    segmentIndex.remove(&seg);
}

CutView::iterator CutView::erase(iterator it) {
  SegmentSet::iterator it_inc = increment.find(*it);
  if(it_inc != increment.end()) {
    increment.erase(it_inc);
    return CutModel::erase(it);
  } else
    return segmentIndex.erase(it);
}

void CutView::clear() {
   for(iterator it = begin(); it != end(); it++) {
     SegmentSet::iterator it_inc = increment.find(*it);
     if(it_inc != increment.end()) {
       increment.erase(it_inc);
       CutModel::erase(it);
     } else
       segmentIndex.erase(it);
   }
 }

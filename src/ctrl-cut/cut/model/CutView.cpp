#if 0
#include "CutView.h"
#include <algorithm>

void CutView::add(Segment& seg) {
  CutModel::clip(seg);
  if (seg.first == seg.second) // ignore zero length segments
    return;
  increment.push_back(&seg);
}

void CutView::remove(Segment& seg) {
  SegmentList::iterator it_seg = std::find(increment.begin(), increment.end(), &seg);
  if(it_seg != increment.end()) {
   increment.remove(&seg);
   delete &seg;
  }
  else
   segmentIndex.remove(&seg);
}

CutView::iterator CutView::erase(iterator it) {
  SegmentList::iterator it_seg = std::find(increment.begin(), increment.end(), *it);
  bool first;
  if(it_seg != increment.end()) {
    first = false;
    Segment* seg = *it_seg;
    it_seg = increment.erase(it_seg);
    delete seg;
  } else {
    first = true;
    it_seg = segmentIndex.erase(it_seg);
  }

  return CutView::iterator(segmentIndex.begin(), segmentIndex.end(), increment.begin(), increment.end(), it_seg, first);
}

void CutView::clear() {
  for(SegmentList::iterator it = increment.begin(); it != increment.end(); it++) {
    Segment* seg = *it;
    increment.erase(it);
    delete seg;
  }

  segmentIndex.clear();
}
#endif

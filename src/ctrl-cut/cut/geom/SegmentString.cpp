#include "SegmentString.h"
#include "cut/geom/Segment.h"
#include "cut/model/CutModel.h"

SegmentString::SegmentString(const CutModel& model) : CutModel (model){}

bool SegmentString::create(const Segment& seg) {
   if (CutModel::empty()) {
     CutModel::create(seg);
     points.push_back(seg.first);
     points.push_back(seg.second);
     return true;
   } else {
     const Point& last = points.back();
     if (last == seg.first) {
       CutModel::create(seg);
       points.push_back(seg.second);
     } else if (last == seg.second) {
       CutModel::create(seg.second, seg.first, *this);
       points.push_back(seg.first);
     } else {
       const Point&  first = points.front();
       if (first == seg.first) {
         CutModel::push_front(Segment(seg.second, seg.first, *this));
         points.push_front(seg.second);
       } else if (first == seg.second) {
         CutModel::push_front(Segment(seg.first, seg.second, *this));
         points.push_front(seg.first);
       } else {
         return false;
       }
     }
     return true;
   }
   return false;
 }

void SegmentString::copy(const SegmentString& other) {
  CutModel::copy(other);
  for(PointConstIter it = other.beginPoints(); it != other.endPoints(); it++) {
    this->points.push_back(*it);
  }
}

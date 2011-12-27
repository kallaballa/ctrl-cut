#include "SegmentString.h"
#include "cut/geom/Segment.h"
#include "cut/model/CutModel.h"

SegmentString::SegmentString(const CutModel& model) : CutModel (model){}

bool SegmentString::append(const Segment& seg) {
   return this->appendBack(seg) && this->appendFront(seg);
 }

bool SegmentString::appendBack(const Segment& seg) {
  if (this->empty()) {
    CutModel::push_back(seg);
    points.push_back(seg.first);
    points.push_back(seg.second);
    return true;
  } else {
    const Point& last = points.back();
    if (last == seg.first) {
      CutModel::push_back(seg);
      points.push_back(seg.second);
    } else if (last == seg.second) {
      CutModel::push_back(Segment(seg.second, seg.first, *this));
      points.push_back(seg.first);
    }
    return false;
  }
}

bool SegmentString::appendFront(const Segment& seg) {
  if (this->empty()) {
    CutModel::push_front(seg);
    std::cerr << this->size() << std::endl;
    points.push_front(seg.first);
    points.push_front(seg.second);
    std::cerr << this->points.size() << std::endl;
    std::cerr << (*this) << std::endl;
    return true;
  } else {
    const Point& first = points.front();
    if (first == seg.first) {
      CutModel::push_front(Segment(seg.second, seg.first, *this));
      points.push_front(seg.second);
      return true;
    } else if (first == seg.second) {
      CutModel::push_front(Segment(seg.first, seg.second, *this));
      points.push_front(seg.first);
      return true;
    }

    return false;
  }
}

bool SegmentString::splice(SegmentString& other) {
  return this->spliceBack(other) && this->spliceFront(other);
}

bool SegmentString::spliceBack(SegmentString& other) {
  if (this->empty()) {
    CutModel::splice(this->end(), *this, other.begin(), other.end());
    points.splice(this->endPoints(), other.points, other.beginPoints(), other.endPoints());
    return true;
  } else {
    const Point& last = points.back();
    if (last == other.frontPoints()) {
      CutModel::splice(this->end(), *this, other.begin(), other.end());
      points.splice(this->endPoints(), other.points, other.beginPoints(), other.endPoints());
      return true;
    } else if (last == other.backPoints()) {
      points.reverse();
      std::reverse(segmentIndex.begin(), segmentIndex.end());
      CutModel::splice(this->begin(), *this, other.begin(), other.end());
      points.splice(this->beginPoints(), other.points, other.beginPoints(), other.endPoints());
      return true;
    }
    return false;
  }
}

bool SegmentString::spliceFront(SegmentString& other) {
  if (this->empty()) {
    CutModel::splice(this->end(), *this, other.begin(), other.end());
    points.splice(this->endPoints(), other.points,  other.beginPoints(), other.endPoints());
    return true;
  } else {
    const Point& first = points.front();
    if (first == other.frontPoints()) {
      points.reverse();
      std::reverse(segmentIndex.begin(), segmentIndex.end());
      CutModel::splice(this->end(), *this, other.begin(), other.end());
      points.splice(this->endPoints(), other.points, other.beginPoints(), other.endPoints());
      return true;
    } else if (first == other.backPoints()) {
      CutModel::splice(this->begin(), *this, other.begin(), other.end());
      points.splice(this->beginPoints(), other.points,other.beginPoints(), other.endPoints());
      return true;
    }
    return false;
  }
}

void SegmentString::push_front(const Segment& seg) {
  this->appendFront(seg);
}

void SegmentString::push_back(const Segment& seg) {
  this->appendBack(seg);
}

void SegmentString::copy(const SegmentString& other) {
  CutModel::copy(other);
  points = points;
}

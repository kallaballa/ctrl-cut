#include "Segment.h"
#include "cut/model/CutModel.h"

Segment::Segment() : SegmentSettings(), parent(NULL) {};
Segment::Segment(CutModel& parent) : SegmentSettings(parent.getSettings()), parent(&parent) {};
Segment::Segment(const Point&  first, const Point&  second, const Segment& seg)
: SegmentSettings(seg), first(first), second(second), parent(NULL), box(NULL), sphere(NULL) {
  make_box_and_sphere(first,second);
}
Segment::Segment(const Point&  first, const Point&  second, CutModel& parent)
: SegmentSettings(parent.getSettings()), first(first), second(second), parent(&parent),  box(NULL), sphere(NULL) {
  make_box_and_sphere(first,second);
}

float Segment::distance(const Point& p) const
{
  return
    fabs((this->first[1] - this->second[1]) * p.x + (this->second[0] - this->first[0]) * p.y +
     this->first[0] * this->second[1] - this->second[0] * this->first[1]) /
    sqrt((this->second[0] - this->first[0]) * (this->second[0] - this->first[0]) +
         (this->second[1] - this->first[1]) * (this->second[1] - this->first[1]));
}

/*!
  Returns angle to the positive Y axis
*/
float Segment::getSlope(bool invert) const
{
  int d_x = (*this)[1][0] - (*this)[0][0];
  int d_y = (*this)[1][1] - (*this)[0][1];
  // make sure we're pointing into the positive halfsphere
  /*if ((*this)[0][0] > (*this)[1][0]) {
    d_x = -d_x;
    d_y = -d_y;
  }*/

  if(invert) {
    d_x = -d_x;
    d_y = -d_y;
  }

  // Swap x and y since we're measuring relative to the Y axis.
  // We also negate the Y axis since positive Y points downwards (left handed)
  return CC_PI - ((float)atan2(d_x, -d_y));
}

void translate(SegmentList& segments, SegmentList::iterator first, SegmentList::iterator last, const Point&  translation) {
  Segment seg;
  int32_t fx;
  int32_t fy;
  int32_t sx;
  int32_t sy;

  for(SegmentList::iterator it = first; it != last; ++it) {
    seg = *it;
    fx = seg.first[0] + translation.x;
    fy = seg.first[1] + translation.y;
    sx = seg.second[0] + translation.x;
    sy = seg.second[1] + translation.y;

    //FIXME
    segments.push_back(Segment(Point(fx,fy),Point(sx,sy), seg));
  }
}

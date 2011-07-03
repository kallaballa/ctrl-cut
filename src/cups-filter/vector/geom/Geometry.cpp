#include "Geometry.h"

float Segment::distance(const Point &p) const
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


#ifndef CUTSEGMENT2D_H_
#define CUTSEGMENT2D_H_

#include <stdint.h>
#include <boost/geometry/geometry.hpp>
#include "CutPoint2D.h"

class CutSegment2D
{
public:
    CutPoint2D first;
    CutPoint2D second;

    CutSegment2D(CutPoint2D& p1, CutPoint2D& p2):
      first(p1),
      second(p2)
    {}

    CutSegment2D() :
      first((* new CutPoint2D())),
      second((* new CutPoint2D()))
    {}
};


#endif /* CUTSEGMENT2D_H_ */

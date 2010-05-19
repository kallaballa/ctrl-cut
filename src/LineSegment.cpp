#include "LineSegment.h"
#include <algorithm>
#include <cmath>

using namespace std;

LineSegment::LineSegment(Joint *start, Joint *end, int power) {
  this->start = start;
  this->end = end;
  this->power = power;
}

LineSegment::~LineSegment() {
  // TODO Auto-generated destructor stub
}

Joint* LineSegment::getStart() {
  return this->start;
}

Joint* LineSegment::getEnd() {
  return this->end;
}

void LineSegment::setStart(Joint* start) {
  this->start = start;
}

void LineSegment::setEnd(Joint* end) {
  this->end = end;
}

void LineSegment::setPower(int power) {
  this->power = power;
}

int LineSegment::getPower() {
  return this->power;
}

void LineSegment::invertDirection() {
  Joint* tmp = getStart();
  setStart(getEnd());
  setEnd(tmp);
}

float LineSegment::getSlope(bool invert) {
  int d_x;
  int d_y;
  if (invert) {
    d_x = start->getX() - end->getX();
    d_y = start->getY() - end->getY();
  } else {
    d_x = end->getX() - start->getX();
    d_y = end->getY() - start->getY();
  }

  //TODO Y flipped
  return atan2(-d_y, d_x);
}

Joint* LineSegment::intersects(LineSegment *otherLine) {
  float denom = ((otherLine->end->getY() - otherLine->start->getY())
                 * (this->end->getX() - this->start->getX()))
    - ((otherLine->end->getX() - otherLine->start->getX())
       * (this->end->getY() - this->start->getY()));

  float nume_a = ((otherLine->end->getX() - otherLine->start->getX())
                  * (this->start->getY() - otherLine->start->getY()))
    - ((otherLine->end->getY() - otherLine->start->getY())
       * (this->start->getX() - otherLine->start->getX()));

  float nume_b = ((this->end->getX() - this->start->getX())
                  * (this->start->getY() - otherLine->start->getY()))
    - ((this->end->getY() - this->start->getY()) * (this->start->getX()
                                                    - otherLine->start->getX()));

  if (denom == 0.0f) {
    if (nume_a == 0.0f && nume_b == 0.0f) {
      return NULL; //COINCIDENCE
    }
    return NULL; //PARALLEL;
  }

  float ua = nume_a / denom;
  float ub = nume_b / denom;

  if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f) {
    // Get the intersection LSPoint.
    int ix = this->start->getX() + ua * (this->end->getX()
                                         - this->start->getX());
    int iy = this->start->getY() + ua * (this->end->getY()
                                         - this->start->getY());

    Joint* intersection = new Joint(ix, iy);
    if ((this->start->equals(intersection) || this->end->equals(
                                                                intersection)) && (otherLine->start->equals(intersection)
                                                                                   || otherLine->end->equals(intersection)))
      return NULL; //tip intersection
    else
      return intersection;

  }

  return NULL; //NOT_INTERSECTING;
}


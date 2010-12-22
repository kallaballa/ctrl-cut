#include "Edge.h"

using namespace std;
int Edge::cnt = 0;

Edge::Edge(Vertex *start, Vertex *end, int power) {
	this->id = cnt++;
	this->start = start;
  this->end = end;
  this->power = power;
  getStart();
}

Edge::~Edge() {
}

Vertex* Edge::getStart() {
  return this->start;
}

Vertex* Edge::getEnd() {
  return this->end;
}

void Edge::setStart(Vertex* start) {
  this->start = start;
}

void Edge::setEnd(Vertex* end) {
  this->end = end;
}

void Edge::setPower(int power) {
  this->power = power;
}

int Edge::getPower() {
  return this->power;
}

void Edge::invertDirection() {
  Vertex* tmp = getStart();
  setStart(getEnd());
  setEnd(tmp);
}

void Edge::detach() {
	  this->getStart()->detach(this);
	  this->getEnd()->detach(this);
}

float Edge::getSlope(bool invert) {
  int d_x;
  int d_y;
  if (invert) {
    d_x = start->getX() - end->getX();
    d_y = start->getY() - end->getY();
  } else {
    d_x = end->getX() - start->getX();
    d_y = end->getY() - start->getY();
  }

  //FIXME should we really flip Y or is that just a workaround?
  return atan2(-d_y, d_x);
}

Vertex* Edge::intersects(Edge *otherLine) {
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

    Vertex* intersection = new Vertex(ix, iy);
    if ((this->start->equals(intersection) || this->end->equals(
                                                                intersection)) && (otherLine->start->equals(intersection)
                                                                                   || otherLine->end->equals(intersection)))
      return NULL; //tip intersection
    else
      return intersection;

  }

  return NULL; //NOT_INTERSECTING;
}

ostream& operator <<(ostream &os, Edge &e) {
	os << "<edge power=\"" << e.power << "\" id=\"" << e.id << "\" >" << std::endl;
	os << *e.start;
	os << *e.end;
	os << "</edge>" << std::endl;
	return os;
}

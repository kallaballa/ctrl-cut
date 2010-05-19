#include "OnionSkin.h"

OnionSkin::OnionSkin() {
  // TODO Auto-generated constructor stub

}

OnionSkin::~OnionSkin() {
  // TODO Auto-generated destructor stub
}

void OnionSkin::addLineSegment(LineSegment* ls) {
  this->segments.push_back(ls);
}

list<LineSegment*> OnionSkin::getLineSegments() {
  return this->segments;
}

void OnionSkin::eraseLineSegment(LineSegment* ls) {
  this->segments.erase(findLineSegment(ls));
}

list<LineSegment*>::iterator OnionSkin::findLineSegment(LineSegment* ls) {
  list<LineSegment*>::iterator it;
  for(it=segments.begin(); it != segments.end(); it++)
    {
      if(*it == ls)
        return it;
    }

  return segments.end();
}


bool OnionSkin::hasLineSegment(LineSegment* ls) {
  return findLineSegment(ls) != segments.end();
}

int OnionSkin::getSegmentCount() {
  return this->segments.size();
}

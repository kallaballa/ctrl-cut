#ifndef POLYLINE_H_
#define POLYLINE_H_

#include "stddef.h"
#include "climits"
#include "iostream"
#include <cmath>
#include "raster/DownSample.h"
#include "VTypes.h"

class Polyline {
public:
  Polyline();
  virtual ~Polyline(){}

  void add(Edge* ls);
  void remove(Edge* ls);
  bool contains(Edge* ls);

  int size();
  Edge *findLeftmostClockwise();
  BBox* getBoundingBox();

  Edge* front() { return this->edges.front(); }
  Edge* back() { return this->edges.back(); }
  VecEdge::iterator begin() { return this->edges.begin(); }
  VecEdge::iterator end() { return this->edges.end(); }
  VecEdge::iterator find(Edge* ls) {
    for (VecEdge::iterator it = this->begin(); it != this->end(); it++) {
      if (*it == ls)
        return it;
    }
    return (VecEdge::iterator)NULL;
  }

  friend std::ostream& operator <<(std::ostream &os, Polyline &pl);

private:
  static int cnt;
  int id;

  VecEdge edges;
  BBox bb;
};

#endif /* POLYGON_H_ */

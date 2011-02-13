#ifndef POLYLINE_H_
#define POLYLINE_H_

#include "stddef.h"
#include "climits"
#include "iostream"
#include <cmath>
#include "raster/DownSample.h"
#include "VTypes.h"

class Polyline : public std::vector<Edge *> {
public:
  Polyline();
  virtual ~Polyline(){}

  void add(Edge* ls);
  void remove(Edge* ls);
  bool contains(Edge* ls);

  Edge *findLeftmostClockwise();
  BBox* getBoundingBox();

  int size() { return this->std::vector<Edge *>::size(); }
  Edge* front() { return this->std::vector<Edge *>::front(); }
  Edge* back() { return this->std::vector<Edge *>::back(); }
  Polyline::iterator begin() { return this->std::vector<Edge *>::begin(); }
  Polyline::iterator end() { return this->std::vector<Edge *>::end(); }
  Polyline::iterator find(Edge* e) {
    for (Polyline::iterator it = this->begin(); it != this->end(); it++) {
      if (*it == e)
        return it;
    }
    return (Polyline::iterator)NULL;
  }

  friend std::ostream& operator <<(std::ostream &os, Polyline &pl);

private:
  static int cnt;
  int id;

  BBox bb;
};

#endif /* POLYGON_H_ */

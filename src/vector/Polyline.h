#ifndef POLYLINE_H_
#define POLYLINE_H_

#include "stddef.h"
#include "climits"
#include "iostream"
#include <cmath>

#include "Primitives.h"

using namespace std;

class Polyline {
public:
	VecEdge edges;
	Polyline();
  virtual ~Polyline(){}

  bool contains(Edge* ls);
  void add(Edge* ls);
  void remove(Edge* ls);
  int count();
  Edge* findSteapest();
  VecEdge::iterator find(Edge* ls);

  friend ostream& operator <<(ostream &os, Polyline &pl);

private:
  static int cnt;
  int id;
};

#endif /* POLYGON_H_ */

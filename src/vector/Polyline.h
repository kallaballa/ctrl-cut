#ifndef POLYGON_H_
#define POLYGON_H_
#include "Edge.h"
#include <vector>
#include "climits"
#include "iostream"
#include <cmath>

using namespace std;

class Polyline {
public:
	set<Edge*> edges;
	Polyline();
  virtual ~Polyline();
  bool contains(Edge* ls);
  void add(Edge* ls);
  void remove(Edge* ls);
  int count();
  Edge* findSteapest();
};

#endif /* POLYGON_H_ */

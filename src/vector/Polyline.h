#ifndef POLYLINE_H_
#define POLYLINE_H_
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
  void xml(ostream &out);
};

#endif /* POLYGON_H_ */

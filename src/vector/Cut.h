#ifndef CUT_H_
#define CUT_H_

#include <fstream>
#include <iostream>
#include "boost/format.hpp"

#include "VTypes.h"

#ifndef VECTOR_POWER_DEFAULT
#define VECTOR_POWER_DEFAULT (80)
#endif

class Cut {
public:
  VecPolyline polylines;
  LstEdge freeEdges;
  MapVertex vertices;

  static Cut* load(const std::string &filename);
  static Cut* load(std::istream &input);

  void createEdge(Vertex *start, Vertex *end, int power);
  void removeEdge(Edge *e, bool detach);
  LstEdge::iterator removeEdge(LstEdge::iterator it_e, bool detach);
  Vertex* mapVertex(Vertex* p);

  bool wasClipped() const {
    return this->clipped;
  }
  void xml(std::string s);
  // Print debug info
  void print(std::ostream &stream);
  friend std::ostream& operator <<(std::ostream &os, Cut &cut);
  Cut() : clipped(false) {}
  virtual ~Cut() {}
private:
  bool clipped;
};


#endif

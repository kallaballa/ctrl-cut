#ifndef CUT_H_
#define CUT_H_

#include <fstream>
#include <iostream>
#include "boost/format.hpp"
#include "Polyline.h"
#include "Mesh.h"

#ifndef VECTOR_POWER_DEFAULT
#define VECTOR_POWER_DEFAULT (80)
#endif

class Cut : public std::vector<Polyline *> {
public:
  Mesh& getMesh() {
    return this->mesh;
  }

  static Cut* load(const std::string &filename);
  static Cut* load(std::istream &input);

  void add(Polyline* ls);
  void remove(Polyline* ls);
  bool contains(Polyline* ls);

  int size() { return this->std::vector<Polyline *>::size(); }
  Polyline* front() { return this->std::vector<Polyline *>::front(); }
  Polyline* back() { return this->std::vector<Polyline *>::back(); }
  Cut::iterator begin() { return this->std::vector<Polyline *>::begin(); }
  Cut::iterator end() { return this->std::vector<Polyline *>::end(); }
  Cut::iterator find(Polyline* e) {
    for (Cut::iterator it = this->begin(); it != this->end(); it++) {
      if (*it == e)
        return it;
    }
    return (Cut::iterator)NULL;
  }
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
  Mesh mesh;
};


#endif

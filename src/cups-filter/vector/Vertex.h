#ifndef VERTEX_H_
#define VERTEX_H_

#include <set>
#include <string>
#include <iostream>
#include <math.h>
#include "util/2D.h"

class Vertex : public Point2D
{
public:
  typedef std::set<class Edge*> EdgeSet;
  typedef EdgeSet::iterator iterator;
  typedef EdgeSet::const_iterator const_iterator;

  Vertex(int x, int y);
  virtual ~Vertex() {}

  iterator begin() { return this->edges.begin(); }
  const_iterator begin() const  { return this->edges.begin(); }
  iterator end() { return this->edges.end(); }
  const_iterator end() const  { return this->edges.end(); }
  size_t size() const { return this->edges.size(); }

  int getID() const { return this->id; }
  const std::string &getKey() const;

  void setValue(int x, int y, bool update=true) {
    this->v[0] = x;
    this->v[1] = y;
    if (update) updateKey();
  }

  void attach(Edge *ls);
  void detach(Edge *ls);
  int distance(const Vertex &other);
private:
  void updateKey();
  int id;
  static int cnt;
  std::string key;

  EdgeSet edges;
};

std::ostream &operator<<(std::ostream &os, const Vertex &v);

#endif

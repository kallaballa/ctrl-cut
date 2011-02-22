#ifndef VERTEX_H_
#define VERTEX_H_

#include <set>
#include <string>
#include <iostream>

class Vertex
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

  int getX() const { return this->x; }
  int getY() const { return this->y; }
  int getID() const { return this->id; }
  const std::string &getKey() const;

  void setX(int x, bool updateKey=true);
  void setY(int y, bool updateKey=true);
  bool operator==(const Vertex &other) const {
    return this->x == other.x && this->y == other.y;
  }
  bool equals(const Vertex *other) const {
    return *this == *other;
  }

  void attach(Edge *ls);
  void detach(Edge *ls);

private:
  void updateKey();

  static int cnt;
  int id;
  int x;
  int y;
  std::string key;

  EdgeSet edges;
};

std::ostream &operator<<(std::ostream &os, const Vertex &v);

#endif

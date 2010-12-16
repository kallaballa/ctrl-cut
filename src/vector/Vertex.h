#ifndef VERTEX_H_
#define VERTEX_H_

#include <set>
#include <string>
#include <cmath>

using namespace std;

class Vertex {
 public:
  Vertex(int x, int y);
  virtual ~Vertex();
  int getX();
  int getY();
  void setX(int x, bool updateKey=true);
  void setY(int y, bool updateKey=true);
  bool equals(Vertex *other);
  string getKey();
  void attach(class Edge *ls);
  void detach(Edge *ls);
  std::set<Edge*> getAttachedEdges();
  void xml(ostream &out);
 private:
  int x;
  int y;
  string key;
  void updateKey();
  std::set<Edge*> attachedEdges;
};

#endif

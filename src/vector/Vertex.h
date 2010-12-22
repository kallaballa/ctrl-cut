#ifndef VERTEX_H_
#define VERTEX_H_

#include <set>
#include <map>
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
  set<Edge*> getAttachedEdges();
  friend ostream& operator <<(ostream &os,const Vertex &v);

 private:
  static int cnt;
  int id;
  int x;
  int y;
  string key;
  set<Edge*> attachedEdges;

  void updateKey();
};

#endif

#ifndef VERTEX_H_
#define VERTEX_H_

#include <set>
#include <string>
#include <cmath>
#include <iostream>

using std::set;
using std::ostream;
using std::string;

class Edge;
class Vertex : public std::set<Edge*> {
public:
  Vertex(int x, int y);
  virtual ~Vertex();
  int getX();
  int getY();
  void setX(int x, bool updateKey=true);
  void setY(int y, bool updateKey=true);
  bool equals(Vertex *other);
  std::string getKey();

  void attach(class Edge *ls);
  void detach(Edge *ls);

  Vertex::iterator find(Edge* e) {
    for (Vertex::iterator it = this->begin(); it != this->end(); it++) {
      if (*it == e)
        return it;
    }
    return (Vertex::iterator) NULL;
  }

  friend ostream& operator <<(ostream &os,const Vertex &v);

private:
  static int cnt;
  int id;
  int x;
  int y;
  std::string key;

  void updateKey();
};

#endif

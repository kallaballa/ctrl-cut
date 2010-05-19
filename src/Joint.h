#ifndef JOINT_H_
#define JOINT_H_

#include <set>
#include <string>
#include <cmath>

using namespace std;

class Joint {
 public:
  Joint(int x, int y);
  virtual ~Joint();
  int getX();
  int getY();
  void setX(int x, bool updateKey=true);
  void setY(int y, bool updateKey=true);
  bool equals(Joint *other);
  string getKey();
  void addConnector(class LineSegment *ls);
  void removeConnector(LineSegment *ls);
  std::set<LineSegment*> getConnectors();

 private:
  int x;
  int y;
  string key;
  void updateKey();
  std::set<LineSegment*> connectors;
};

#endif

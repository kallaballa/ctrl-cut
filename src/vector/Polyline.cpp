#include "Polyline.h"


using namespace std;

Polyline::Polyline() {
}

Polyline::~Polyline() {
}

void Polyline::add(Edge* ls) {
	edges.insert(ls);
}

bool Polyline::contains(Edge* ls) {
	return edges.find(ls) != edges.end();
}

void Polyline::remove(Edge* ls) {
	edges.erase(ls);
}

int Polyline::count() {
	return edges.size();
}

Edge* Polyline::findSteapest() {
  Edge* ls;
  int min_x = INT_MAX;
  Vertex* most_left = NULL;
  set<Edge*>::iterator it;
  int sx;
  int ex;

  for (it = edges.begin(); it != edges.end(); it++) {
    ls = *it;
    sx = ls->getStart()->getX();
    ex = ls->getEnd()->getX();

    if (sx < min_x) {
      min_x = sx;
      most_left = ls->getStart();
    }

    if (ex < min_x) {
      min_x = ex;
      most_left = ls->getEnd();
    }
  }

  set<Edge*> connected = most_left->getAttachedEdges();

  Edge* edge;
  float slope;
  float steapest = M_PI;

  for (it = connected.begin(); it != connected.end(); it++) {
    ls = *it;

    if (ls->getStart()->getY() > ls->getEnd()->getY())
      ls->invertDirection();

    slope = abs(ls->getSlope() - (M_PI / 2));
    if (slope >= 3.1415f)
      slope = 0;

    if (slope < steapest) {
      steapest = slope;
      edge = ls;
    }
  }

  if (ls->getSlope() < 0)
    ls->invertDirection();

  return edge;
}

void Polyline::xml(ostream &out) {
	out << "<polyline cnt=\"" << this->count() << "\" id=\"" << this << "\" >" << std::endl;
	for(set<Edge*>::iterator it = this->edges.begin(); it != this->edges.end(); it++) {
		((Edge*)*it)->xml(out);
	}
	out << "</polyline>" << std::endl;
}

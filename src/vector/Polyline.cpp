#include "Polyline.h"
#include "Edge.h"

using namespace std;

int Polyline::cnt = 0;

Polyline::Polyline() { this->id++; }

void Polyline::add(Edge* ls) {
	edges.push_back(ls);
}

VecEdge::iterator Polyline::find(Edge* ls) {
	for (VecEdge::iterator it = this->edges.begin(); it 	!= this->edges.end(); it++) {
		if (*it == ls)
			return it;
	}

	return (VecEdge::iterator)NULL;
}

bool Polyline::contains(Edge* ls) {
	VecEdge::iterator it = find(ls);
	return  it != (VecEdge::iterator) NULL && it != edges.end();
}

void Polyline::remove(Edge* ls) {
	VecEdge::iterator it = find(ls);
	if(it != (VecEdge::iterator) NULL)
		edges.erase(it);
}

int Polyline::count() {
	return edges.size();
}

Edge* Polyline::findSteapest() {
	Edge* ls;
	int min_x = INT_MAX;
	Vertex* most_left = NULL;
	VecEdge::iterator it;
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
	SetEdge::iterator it_c;
	SetEdge connected = most_left->getAttachedEdges();

	Edge* edge;
	float slope;
	float steapest = M_PI;

	for (it_c = connected.begin(); it_c != connected.end(); it_c++) {
		ls = *it_c;

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

ostream& operator <<(ostream &os, Polyline &pl) {
	os << "<polyline id=\"" << pl.id << "\" >"
			<< std::endl;

	for (VecEdge::iterator it = pl.edges.begin(); it
			!= pl.edges.end(); it++) {
		os << *((Edge*) *it);
	}
	os << "</polyline>" << std::endl;
	return os;
}

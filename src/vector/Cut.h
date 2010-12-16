#ifndef CUT_H_
#define CUT_H_


#include <fstream>
#include <iostream>
#include "boost/format.hpp"

#include "Primitives.h"

#ifndef VECTOR_POWER_DEFAULT
#define VECTOR_POWER_DEFAULT (80)
#endif

class Cut {
public:
	PolylineVector polylines;
	EdgeList freeEdges;
	VertexMap vertices;

	void createEdge(Vertex *start, Vertex *end, int power);
	void removeEdge(Edge *e, bool detach);
	EdgeList::iterator removeEdge(EdgeList::iterator it_e, bool detach);
	void load(const string &filename);
	Vertex* mapVertex(Vertex* p);

	bool wasClipped() const {
		return this->clipped;
	}
	void xml(std::string s);
	void xml(ostream &out);
	// Print debug info
	void print(ostream &stream);
	Cut() : clipped(false) {}
private:
	//TODO: super inefficent string based key comparator


	bool clipped;

	virtual ~Cut() {}
};

#endif

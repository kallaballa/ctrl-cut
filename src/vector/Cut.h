#ifndef VECTORPASS_H_
#define VECTORPASS_H_

#include <map>
#include "Primitives.h"

#ifndef VECTOR_POWER_DEFAULT
#define VECTOR_POWER_DEFAULT (80)
#endif

class Cut {
public:
	PolylineVector polylines;
	EdgeList freeEdges;

	void createEdge(Vertex *start, Vertex *end, int power);
	void removeEdge(Edge *e);
	void load(const string &filename);
	Vertex* mapVertex(Vertex* p);

	bool wasClipped() const {
		return this->clipped;
	}

	// Print debug info
	void print(ostream &stream);
	Cut() : clipped(false) {}
private:
	//TODO: super inefficent string based key comparator
	typedef std::map<string, class Vertex *> VertexMap;
	VertexMap vertices;
	bool clipped;

	virtual ~Cut() {}
};

#endif

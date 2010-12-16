#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "Edge.h"
#include "Polyline.h"
#include "Vertex.h"
#include <list>
#include <map>

typedef list<class Edge*> EdgeList;
typedef vector<Polyline*> PolylineVector;
typedef map<string, class Vertex *> VertexMap;

#endif /* PRIMITIVES_H_ */

#ifndef V_TYPES_H_
#define V_TYPES_H_

#include <set>
#include <list>
#include <vector>
#include <map>

#include "Edge.h"
#include "Polyline.h"
#include "Vertex.h"

#endif /* V_TYPES_H_ */
using namespace std;

typedef vector<class Polyline*> VecPolyline;
typedef set<class Edge*> SetEdge;
typedef list<class Edge*> LstEdge;
typedef vector<class Edge*> VecEdge;
typedef map<string, class Vertex *> MapVertex;

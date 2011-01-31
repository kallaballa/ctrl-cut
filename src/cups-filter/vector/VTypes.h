#ifndef V_TYPES_H_
#define V_TYPES_H_

#include <set>
#include <list>
#include <vector>
#include <map>
#include <string>

class Vertex;

typedef std::vector<class Polyline*> VecPolyline;
typedef std::set<class Edge*> SetEdge;
typedef std::list<class Edge*> LstEdge;
typedef std::vector<class Edge*> VecEdge;
typedef std::map<std::string, class Vertex *> MapVertex;

#endif /* V_TYPES_H_ */

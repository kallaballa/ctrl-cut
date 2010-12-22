#include "Join.h"
#include <boost/format.hpp>

using boost::format;

Join::~Join() {
  // TODO Auto-generated destructor stub
}

void find_connected(set<Edge*> *occupied, Polyline *polyline,
                    Edge* current) {
  set<Edge*> connectors = current->getEnd()->getAttachedEdges();
  set<Edge*>::iterator it;
  Edge* candidate;

  occupied->insert(current);

  for (it = connectors.begin(); it != connectors.end(); it++) {
    candidate = *it;
    if (candidate == current || occupied->find(candidate) != occupied->end())
      continue;
    if (candidate->getStart() != current->getEnd()) {
      candidate->setEnd(candidate->getStart());
      candidate->setStart(current->getEnd());
    }
    polyline->add(candidate);
    find_connected(occupied, polyline, candidate);
  }
}

void Join::filter(Cut *cut)
{
  std::cerr << "Join" << std::endl;

  set<Edge*> *occupied = new set<Edge*> ();

  Edge* ls;
  list<Edge*>::iterator it;
  int cnt = 0;
  for (it = cut->freeEdges.begin(); it != cut->freeEdges.end(); it++) {
    ls = *it;

		Polyline *polyline = new Polyline();

    if (occupied->find(ls) == occupied->end()) {
      polyline->add(ls);
      find_connected(occupied, polyline, ls);
      cut->polylines.push_back(polyline);
    }
    cnt++;
  }

	for(vector<Polyline*>::iterator it = cut->polylines.begin(); it != cut->polylines.end(); it++) {
		Polyline *p = *it;
		for(vector<Edge*>::iterator it_e = p->edges.begin(); it_e != p->edges.end(); it_e++) {
			cut->removeEdge(*it_e, false);
		}
	}
}

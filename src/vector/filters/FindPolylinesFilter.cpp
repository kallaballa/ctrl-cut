#include "FindPolylinesFilter.h"

FindPolylinesFilter::~FindPolylinesFilter() {
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

void FindPolylinesFilter::filter(Cut *cut)
{
  std::cerr << "FindPolyLines" << std::endl;

  set<Edge*> *occupied = new set<Edge*> ();

  Edge* ls;
  list<Edge*>::iterator it;

  for (it = cut->freeEdges.begin(); it != cut->freeEdges.end(); it++) {
    ls = *it;

    Polyline *polyline = new Polyline();

    if (occupied->find(ls) == occupied->end()) {
      polyline->add(ls);
      cut->removeEdge(ls);
      find_connected(occupied, polyline, ls);
      cut->polylines.push_back(polyline);
    }
  }
}

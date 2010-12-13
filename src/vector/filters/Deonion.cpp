#include "Deonion.h"


Deonion::~Deonion() {
  // TODO Auto-generated destructor stub
}

void walkTheEdge(Polyline* p, Polyline* skin, Edge* edge, bool cw) {
  set<Edge*> connectors = edge->getEnd()->getAttachedEdges();
  set<Edge*>::iterator it;
  Edge* candidate;
  Edge* next_edge = NULL;

  float edge_slope = edge->getSlope(true);
  float candidate_slope;

  float slope_diff;
  float min_slope_diff = 2 * M_PI;

  //TODO resolve double check
  if(p->contains(edge))
    skin->add(edge);

  p->remove(edge);

  for (it = connectors.begin(); it != connectors.end(); it++) {
    candidate = *it;

    if (candidate == edge || !p->contains(candidate))
      continue;

    if (candidate->getStart() != edge->getEnd())
      candidate->invertDirection();

    candidate_slope = candidate->getSlope();

    slope_diff = edge_slope - candidate_slope;
    if(slope_diff < 0)
      slope_diff+=2*M_PI;
    else if(slope_diff > 2*M_PI)
      slope_diff-=2*M_PI;

    if (slope_diff < min_slope_diff) {
      min_slope_diff = slope_diff;
      next_edge = candidate;
    }
  }

  if (next_edge == NULL && cw) {
    edge->invertDirection();
    walkTheEdge(p, skin, edge, !cw);
  } else if (next_edge != NULL) {
    if(!cw)
      cw = true;
    walkTheEdge(p, skin, next_edge, cw);
  }
}

void Deonion::filter(Cut *cut)
{
  std::cerr << "Deonion" << std::endl;
  
  unsigned int i;
  Polyline* p;
  PolylineVector skins;

  for (i = 0; i < cut->polylines.size(); i++) {
    p = cut->polylines.at(i);

    while (p->count() > 0) {
      Polyline *skin = new Polyline();
      walkTheEdge(p, skin, p->findSteapest(), true);
      skins.push_back(skin);
    }

    cut->polylines = skins;
  }
}

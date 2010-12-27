#include "Deonion.h"
#include "../VTypes.h"
#include "../Edge.h"
#include "../Polyline.h"
#include "../Cut.h"

Deonion::~Deonion() {
}

void walkTheEdge(Polyline* p, Polyline* skin, Edge* edge, bool cw) {
  SetEdge connectors = edge->getEnd()->getAttachedEdges();
  SetEdge::iterator it;
  Edge* candidate;
  Edge* next_edge = NULL;

  float edge_slope = edge->getSlope(true);
  float candidate_slope;

  float slope_diff;
  float min_slope_diff = 2 * M_PI;

  //TODO resolve double check
  if (p->contains(edge)) {
    skin->add(edge);
    p->remove(edge);
  }

  for (it = connectors.begin(); it != connectors.end(); it++) {
    candidate = *it;

    if (candidate == edge || !p->contains(candidate)) {
      continue;
    }

    if (candidate->getStart() != edge->getEnd()) {
      candidate->invertDirection();
    }

    candidate_slope = candidate->getSlope();

    slope_diff = edge_slope - candidate_slope;
    if (slope_diff < 0) {
      slope_diff += 2*M_PI;
    }
    else if (slope_diff > 2*M_PI) {
      slope_diff -= 2*M_PI;
    }

    if (slope_diff < min_slope_diff) {
      min_slope_diff = slope_diff;
      next_edge = candidate;
    }
  }

  if (next_edge == NULL && cw) {
    edge->invertDirection();
    walkTheEdge(p, skin, edge, !cw);
  } else if (next_edge != NULL) {
    if (!cw) {
      cw = true;
    }
    walkTheEdge(p, skin, next_edge, cw);
  }
}

void Deonion::filter(Cut *cut)
{
  std::cerr << "Deonion: " << cut->polylines.size() << std::endl;
  
  unsigned int i;
  Polyline* p;
  VecPolyline skins;

  for (i = 0; i < cut->polylines.size(); i++) {
    p = cut->polylines.at(i);
    if (i % 100 == 0) {
    	std::cerr << i << std::endl;
    }
    while (p->count() > 0) {
      Polyline *skin = new Polyline();
      walkTheEdge(p, skin, p->findSteapest(), true);
      skins.push_back(skin);
    }

  }
  cut->polylines = skins;
}

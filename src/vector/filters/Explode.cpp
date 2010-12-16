#include "Explode.h"
#include <string>

using boost::format;

Explode::~Explode() {
}

void Explode::filter(Cut *cut) {
	std::cerr << "Split" << std::endl;

	Vertex *intersec = NULL;
	Edge *ls1, *ls2;

	list<Edge*>::iterator it_i;
	list<Edge*>::iterator it_j;

	size_t numLines = cut->freeEdges.size();
	unsigned int percent = numLines / 100;
	unsigned int cntLines = 0;
	for (it_i = cut->freeEdges.begin(); it_i != cut->freeEdges.end(); it_i++) {
		if (cntLines % percent == 0)
			std::cerr << cntLines / percent << std::endl;
		cntLines++;

		for (it_j = cut->freeEdges.begin(); it_j != cut->freeEdges.end(); it_j++) {

			ls2 = *it_j;
			ls1 = *it_i;

			if (it_i == cut->freeEdges.end())
				break;

			if (ls1 == ls2)
				continue;

			if ((intersec = ls1->intersects(ls2)) != NULL) {
				intersec = cut->mapVertex(intersec);

				if (!ls1->getStart()->equals(intersec) && !ls1->getEnd()->equals(
						intersec)) {
					it_i = cut->removeEdge(it_i, true);
					cut->createEdge((Vertex*) ls1->getStart(), intersec, ls1->getPower());
					cut->createEdge((Vertex*) ls1->getEnd(), intersec, ls1->getPower());
				}

				if (!ls2->getStart()->equals(intersec) && !ls2->getEnd()->equals(
						intersec)) {
					it_j = cut->removeEdge(it_j, true);
					cut->createEdge((Vertex*) ls2->getStart(), intersec, ls2->getPower());
					cut->createEdge((Vertex*) ls2->getEnd(), intersec, ls2->getPower());
				}
			}
		}
	}
}

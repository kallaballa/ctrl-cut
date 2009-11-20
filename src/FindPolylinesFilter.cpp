/*
 * FindPolylinesFilter.cpp
 *
 *  Created on: 12.11.2009
 *      Author: amir
 */

#include "FindPolylinesFilter.h"
#include "LineSegment.h"
#include "Polyline.h"
#include <list>
#include <set>

FindPolylinesFilter::~FindPolylinesFilter() {
	// TODO Auto-generated destructor stub
}

void find_connected(set<LineSegment*> *occupied, Polyline *polyline,
        LineSegment* current) {
    set<LineSegment*> connectors = current->getEnd()->getConnectors();
    set<LineSegment*>::iterator it;
    LineSegment* candidate;

    occupied->insert(current);

    for (it = connectors.begin(); it != connectors.end(); it++) {
        candidate = *it;
        if (candidate == current || occupied->find(candidate) != occupied->end())
            continue;
        if (candidate->getStart() != current->getEnd()) {
            candidate->setEnd(candidate->getStart());
            candidate->setStart(current->getEnd());
        }
        polyline->addLineSegment(candidate);
        find_connected(occupied, polyline, candidate);
    }
}


void FindPolylinesFilter::filter(VectorPass* vpass) {
    set<LineSegment*> *occupied = new set<LineSegment*> ();

    LineSegment* ls;
    list<LineSegment*>::iterator it;

    for (it = vpass->lines.begin(); it != vpass->lines.end(); it++) {
        ls = *it;

        Polyline *polyline = new Polyline();

        if (occupied->find(ls) == occupied->end()) {
            polyline->addLineSegment(ls);
            find_connected(occupied, polyline, ls);
            vpass->polylines.push_back(polyline);
        }
    }
}

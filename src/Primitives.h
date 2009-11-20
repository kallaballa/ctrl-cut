/*
 * Primitives.h
 *
 *  Created on: 15.11.2009
 *      Author: amir
 */

#ifndef PRIMITIVES_H_
#define PRIMITIVES_H_

#include "OnionSkin.h"
#include "LineSegment.h"
#include "Polyline.h"
#include "Joint.h"

typedef map<string, class Joint *> JointMap;
typedef list<class LineSegment*> LineSegmentList;
typedef vector<class OnionSkin*> OnionSkinVector;
typedef vector<Polyline*> PolylineVector;

#endif /* PRIMITIVES_H_ */

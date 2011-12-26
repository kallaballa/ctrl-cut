#ifndef DEONION_H_
#define DEONION_H_

#include "cut/model/CutModel.h"
#include "cut/geom/SegmentString.h"
#include "cut/geom/Route.h"

void traverse_onion(Route& skins, CutModel::iterator first, CutModel::iterator last);

#endif

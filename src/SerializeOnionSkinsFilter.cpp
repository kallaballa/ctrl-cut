/*
 * SerializeOnionSkinsFilter.cpp
 *
 *  Created on: 15.11.2009
 *      Author: amir
 */

#include "SerializeOnionSkinsFilter.h"
#include "Primitives.h"

SerializeOnionSkinsFilter::~SerializeOnionSkinsFilter() {}

void SerializeOnionSkinsFilter::filter(VectorPass* vpass) {
    vpass->lines.clear();

    OnionSkin* skin;
    for (OnionSkinVector::iterator it = vpass->skins.begin(); it != vpass->skins.end(); it++) {
        skin = *it;

        for (LineSegmentList::iterator it_s = skin->segments.begin(); it_s != skin->segments.end(); it_s++)
            vpass->lines.push_back(*it_s);
    }
}

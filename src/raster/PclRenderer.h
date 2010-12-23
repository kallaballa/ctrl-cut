/*
 * Renderer.h
 *
 *  Created on: Dec 13, 2010
 *      Author: elchaschab
 */

#ifndef PCL_RENDERER_H_
#define PCL_RENDERER_H_

#include <map>
#include "boost/format.hpp"

#include "../util/LaserConfig.h"
#include "../LaserJob.h"
#include "RTypes.h"
#include "Raster.h"

class PclRenderer {

public:
	LaserConfig *lconf;

	void renderRaster(Raster *raster, ostream &out);
	void renderTile(Tile* tile, ostream& out);

	PclRenderer(LaserConfig *lconf);
	virtual ~PclRenderer();
};

#endif /* PCL_RENDERER_H_ */

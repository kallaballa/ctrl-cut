/*
 * Renderer.h
 *
 *  Created on: Dec 13, 2010
 *      Author: elchaschab
 */

#ifndef RENDERER_H_
#define RENDERER_H_

#include <map>

#include "../util/LaserConfig.h"
#include "../LaserJob.h"
#include "Primitives.h"
#include "Cut.h"

class Renderer {

public:
	laser_config *lconf;
	void renderCut(Cut *cut, ostream &out);
	Renderer(laser_config *lconf);
	virtual ~Renderer();
};

#endif /* RENDERER_H_ */

/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef NEARESTPATHSORTING_H_
#define NEARESTPATHSORTING_H_

#include "cut/geom/Route.hpp"

void nearest_path_sorting(const Route& src, Route& sink);

#endif /* NEARESTPATHSORTING_H_ */

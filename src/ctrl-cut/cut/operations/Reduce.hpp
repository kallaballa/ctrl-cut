/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef REDUCE_H_
#define REDUCE_H_

#include "cut/geom/Route.hpp"

void reduce(Route& src, Route& sink, double maxDistance);

#endif

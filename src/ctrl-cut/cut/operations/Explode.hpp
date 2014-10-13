/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef EXPLODE_H_
#define EXPLODE_H_

#include "cut/geom/Route.hpp"

void explode(Route& src, Route& sink);

#endif

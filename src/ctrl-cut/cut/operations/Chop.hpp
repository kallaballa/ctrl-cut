/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef CHOP_H_
#define CHOP_H_

#include "cut/geom/Route.hpp"

void chop(const Route& src, Route& sink, Coord_t maxLength);
void chop(const Route& src, Route& sink, Coord_t maxAmplitudeXum, Coord_t maxAmplitudeYum);

#endif

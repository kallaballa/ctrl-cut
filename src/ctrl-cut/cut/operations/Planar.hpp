/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef PLANAR_H_
#define PLANAR_H_

#include "cut/geom/Route.hpp"

void make_planar_faces(const Route& src, Route& sink);

#endif /* PLANAR_H */

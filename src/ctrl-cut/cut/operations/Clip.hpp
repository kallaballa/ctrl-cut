/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef CLIP_HPP_
#define CLIP_HPP_

#include "cut/geom/Route.hpp"

void clip(Route& src, Route& sink, const Box& bounds);

#endif /* CLIP_HPP_ */

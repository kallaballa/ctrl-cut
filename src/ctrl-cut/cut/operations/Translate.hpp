/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef _TRANSLATE_HPP_
#define _TRANSLATE_HPP_

#include "cut/geom/Route.hpp"

void translateTo(const Route& src, Route& sink, const Point& translate);

#endif

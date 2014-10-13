/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */


#ifndef TRAVELLER_HPP_
#define TRAVELLER_HPP_

#include "cut/geom/Route.hpp"

void travel(const Route& src, Route& sink);

#endif /* TRAVELLER_HPP_ */

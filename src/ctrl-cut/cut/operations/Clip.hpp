#ifndef CLIP_HPP_
#define CLIP_HPP_

#include "cut/geom/Route.hpp"

void clip(Route& src, Route& sink, const Box& bounds);

#endif /* CLIP_HPP_ */

/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef HPGL_ENCODER_H_
#define HPGL_ENCODER_H_

#include <iostream>
#include "cut/Cut.hpp"

class HPGLEncoder {

public:
  static void encode(std::ostream &out, Cut& model);
};

#endif /* HPGL_ENCODER_H_ */

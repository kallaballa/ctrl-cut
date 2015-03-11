/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef HPGL_FUSION_ENCODER_H_
#define HPGL_FUSION_ENCODER_H_

#include <iostream>
#include "cut/Cut.hpp"
#include "cutters/epilog/encoder/HPGL_Legend36Ext_Encoder.hpp"

class HPGL_Fusion_Encoder : public HPGL_Legend36Ext_Encoder {
private:
  virtual void startPass(std::ostream &out, Cut& encodee);

public:
};

#endif /* HPGL_FUSION_ENCODER_H_ */

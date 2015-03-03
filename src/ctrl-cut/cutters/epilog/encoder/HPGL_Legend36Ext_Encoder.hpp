/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef HPGL_LEGEND36EXT_ENCODER_H_
#define HPGL_LEGEND36EXT_ENCODER_H_

#include <iostream>
#include "cut/Cut.hpp"

class HPGL_Legend36Ext_Encoder {
private:
  virtual void startPass(std::ostream &out, Cut& encodee);
  virtual void startPolygon(std::ostream &out, int startX, int startY, int endX, int endY);
  virtual void continuePolygon(std::ostream &out, int x, int y);
  virtual void closePolygon(std::ostream &out, int x, int y);

public:
  virtual void encode(std::ostream &out, Cut& model);

  bool firstOperation = true;
  bool writingPolyline = false;
};

#endif /* HPGL_LEGEND36EXT_ENCODER_H_ */

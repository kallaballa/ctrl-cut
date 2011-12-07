#ifndef HPGL_ENCODER_H_
#define HPGL_ENCODER_H_

#include <iostream>
#include "model/CutModel.h"

class HPGLEncoder {

public:
  void encode(std::ostream &out, CutModel& model) const;
  HPGLEncoder() {};
  virtual ~HPGLEncoder() {};
};

#endif /* HPGL_ENCODER_H_ */

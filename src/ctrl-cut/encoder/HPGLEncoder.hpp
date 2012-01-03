#ifndef HPGL_ENCODER_H_
#define HPGL_ENCODER_H_

#include <iostream>
#include "cut/model/CutModel.hpp"

class HPGLEncoder {

public:
  static void encode(std::ostream &out, CutModel& model);
};

#endif /* HPGL_ENCODER_H_ */

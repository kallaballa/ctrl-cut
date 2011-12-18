#ifndef HPGL_ENCODER_H_
#define HPGL_ENCODER_H_

#include <iostream>

class HPGLEncoder {

public:
  static void encode(std::ostream &out, class CutModel& model);
};

#endif /* HPGL_ENCODER_H_ */

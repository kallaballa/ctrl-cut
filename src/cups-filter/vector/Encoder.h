#ifndef ENCODER_H_
#define ENCODER_H_

#include "vector/model/CutModel.h"

class Encoder {

public:
  Encoder(class LaserConfig *lconf) : lconf(lconf) {}
  virtual ~Encoder() {}

  virtual void encode(CutModel& model, std::ostream &out) const = 0;

protected:
  LaserConfig *lconf;
};

#endif /* RENDERER_H_ */

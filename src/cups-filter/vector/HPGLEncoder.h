#ifndef RENDERER_H_
#define RENDERER_H_

#include <map>

#include "util/LaserConfig.h"
#include "LaserJob.h"
#include "vector/model/CutModel.h"

class HPGLEncoder {

public:
  LaserConfig *lconf;
  void encode(CutModel& cut, std::ostream &out);
  HPGLEncoder(LaserConfig *lconf);
  virtual ~HPGLEncoder();
};

#endif /* RENDERER_H_ */

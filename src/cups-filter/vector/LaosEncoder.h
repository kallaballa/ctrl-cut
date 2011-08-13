#ifndef RENDERER_H_
#define RENDERER_H_

#include <map>

#include "util/LaserConfig.h"
#include "LaserJob.h"
#include "vector/model/CutModel.h"

class LaosEncoder {

public:
  LaserConfig *lconf;
  void encode(CutModel& model, std::ostream &out) const;
  LaosEncoder(LaserConfig *lconf);
  virtual ~LaosEncoder();
};

#endif /* RENDERER_H_ */

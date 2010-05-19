#ifndef LASER_PASS_H_
#define LASER_PASS_H_

#include <string>
#include <list>
#include <map>
#include <vector>
#include <iostream>

#include "laser_config.h"
#include "Raster.h"
#include "TilePartitioner.h"
#include "Primitives.h"

using std::map;
using std::list;
using std::vector;
using std::string;
using std::ostream;

//stub
struct pass_params {
};

class LaserPass {
  pass_params params;

 public:
  laser_config *lconf;
  LaserPass() {}
  LaserPass(pass_params params) : params(params) {}
  virtual ~LaserPass() {}
};

#endif

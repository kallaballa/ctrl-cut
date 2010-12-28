#ifndef RENDERER_H_
#define RENDERER_H_

#include <map>

#include "../util/LaserConfig.h"
#include "../LaserJob.h"
#include "VTypes.h"
#include "Cut.h"

class Renderer {

public:
  LaserConfig *lconf;
  void renderCut(Cut *cut, std::ostream &out);
  Renderer(LaserConfig *lconf);
  virtual ~Renderer();
};

#endif /* RENDERER_H_ */

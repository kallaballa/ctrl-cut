#ifndef RENDERER_H_
#define RENDERER_H_

#include <map>

#include "util/LaserConfig.h"
#include "LaserJob.h"
#include "vector/model/CutModel.h"

class HPGLEncoder {

public:
  LaserConfig *lconf;
  void encode(StringList::const_iterator first, StringList::const_iterator last, std::ostream &out, const bool wasClipped) const;
  HPGLEncoder(LaserConfig *lconf);
  virtual ~HPGLEncoder();
};

#endif /* RENDERER_H_ */

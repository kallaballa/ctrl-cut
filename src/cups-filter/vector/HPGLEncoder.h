#ifndef HPGLENCODER_H_
#define HPGLENCODER_H_

#include "Encoder.h"

class HPGLEncoder : public Encoder
{
public:
  HPGLEncoder(LaserConfig *lconf);
  ~HPGLEncoder();

  void encode(CutModel& model, std::ostream &out) const;
};

#endif /* RENDERER_H_ */

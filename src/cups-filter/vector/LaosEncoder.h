#ifndef LAOSENCODER_H_
#define LAOSENCODER_H_

#include "Encoder.h"

class LaosEncoder : public Encoder
{
public:
  LaosEncoder(LaserConfig *lconf) : Encoder(lconf) {}
  ~LaosEncoder() {}
  void encode(CutModel& model, std::ostream &out) const;
};

#endif

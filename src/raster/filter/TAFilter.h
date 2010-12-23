#ifndef TAFILTER_H_
#define TAFILTER_H_

#include "../Raster.h"
#include "../DownSample.h"

#include <list>

class TAFilter {
 private:

 public:
  TAFilter() {}
  virtual ~TAFilter(){}
  virtual void filter(Raster* raster);
};

#endif /* TAFILTER_H_ */

#ifndef TPFILTER_H_
#define TPFILTER_H_
#include "../Raster.h"
#include "../DownSample.h"

#include <list>

using std::list;

class TPFilter {
private:

public:
  TPFilter() {}
  virtual ~TPFilter(){}
  virtual void filter(class Raster* raster);
};

#endif /* TPFILTER_H_ */

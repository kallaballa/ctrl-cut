#ifndef LASERJOB_H_
#define LASERJOB_H_

#include <list>
#include <map>
#include <string>
#include "stdint.h"
#include <iostream>
#include "boost/format.hpp"

#include "util/PJL.h"
#include "util/LaserConfig.h"
#include "vector/LaosEncoder.h"
#include "vector/HPGLEncoder.h"
#include "raster/Raster.h"
#include "raster/PclEncoder.h"
#include "vector/model/CutModel.h"

using std::string;

class LaserJob
{
public:
  string name;   /*! Job name for the print. */
  string user;   /*! User name that submitted the print job. */
  string title;  /*! Title for the print job. */

  std::list<CutModel*> cuts;
  std::list<Raster*> rasters;
  LaserConfig *lconf;

  LaserJob(LaserConfig *lconf, 
           const string &user, const string &name, const string &title);
  virtual ~LaserJob();

  void optimize();
  void addCut(CutModel* cut);
  void addRaster(Raster* raster);
  std::list<CutModel*> getCuts() { return cuts; }

  void serializeTo(std::ostream &out);
};

#endif /* LASERJOB_H_ */

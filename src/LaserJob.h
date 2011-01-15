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
#include "vector/Cut.h"
#include "vector/Renderer.h"
#include "raster/Raster.h"
#include "raster/PclRenderer.h"

using std::string;

class LaserJob
{
public:
  string name;   /*! Job name for the print. */
  string user;   /*! User name that submitted the print job. */
  string title;  /*! Title for the print job. */

  list<Cut*> cuts;
  list<Raster*> rasters;
  LaserConfig *lconf;

  LaserJob(LaserConfig *lconf, 
           const string &user, const string &name, const string &title);
  virtual ~LaserJob();

  void optimize();
  void addCut(Cut* cut);
  void addRaster(Raster* raster);
  list<Cut*> getCuts() { return cuts; }

  void serializeTo(ostream &out);
};

#endif /* LASERJOB_H_ */

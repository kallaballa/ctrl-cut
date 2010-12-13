#ifndef LASERJOB_H_
#define LASERJOB_H_

#include <list>
#include <map>
#include <string>

#include "laser_config.h"
#include "Laser.h"
#include "vector/Cut.h"
#include "pjl.h"
#include "stdint.h"

using std::string;

/**
 * Convert a big endian value stored in the array starting at the given pointer
 * position to its little endian value.
 *
 * @param position the starting location for the conversion. Each successive
 * unsigned byte is upto nbytes is considered part of the value.
 * @param nbytes the number of successive bytes to convert.
 *
 * @return An integer containing the little endian value of the successive
 * bytes.
 */
static int big_to_little_endian(uint8_t *position, int nbytes) {
  int i;
  int result = 0;

  for (i = 0; i < nbytes; i++) {
    result += *(position + i) << (8 * i);
  }
  return result;
}

class LaserJob
{
 public:
  string name;   /*! Job name for the print. */
  string user;   /*! User name that submitted the print job. */
  string title;  /*! Title for the print job. */

  list<Cut*> cuts;
  laser_config *lconf;

  LaserJob(laser_config *lconf, 
           const string &user, const string &name, const string &title);
  virtual ~LaserJob();

  void optimize();
  void addCut(Cut* cut);
  list<Cut*> getCuts() { return cuts; }

  void serializeTo(ostream &out);
};

#endif /* LASERJOB_H_ */

#ifndef LASERJOB_H_
#define LASERJOB_H_

#include <list>
#include <map>
#include <string>

#include "laser_config.h"
#include "Laser.h"
#include "LaserPass.h"
#include "LineSegment.h"
#include "OnionSkin.h"
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

typedef list<LaserPass*> LaserPassList;

class LaserJob {
 public:
  /** Job name for the print. */
  string name;

  /** User name that submitted the print job. */
  string user;

  /** Title for the job print. */
  string title;


  LaserPassList passes;
  laser_config *lconf;

  LaserJob(laser_config *lconf,
           const string &user, const string &name, const string &title);
  virtual ~LaserJob();

  void optimize();
  void addPass(LaserPass* pass);
  LaserPassList &getPasses() { return passes; }

  void serializeTo(ostream &out);
};

#endif /* LASERJOB_H_ */

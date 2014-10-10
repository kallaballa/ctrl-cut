
#ifndef LASERCUTTER_H_
#define LASERCUTTER_H_

#include "util/Logger.hpp"
#include "util/Measurement.hpp"

/**
 *  This class models the basic hardware specific parameters of a laser cutter.
 *  At the moment its a static configuration supporting only two laser cutters.
 *
 *  TODO: load laser cutter specifications from file.
 */
class LaserCutter {
private:
  // singleton - only one laser cutter model per ctrl-cut instance.
  static LaserCutter* instance;

public:
  enum Driver {
    UNINITIALIZED,
    EPILOG_LEGEND,
    EPILOG_ZING
  };

  Driver driver;
  Unit nativeUnit;
  bool supportsAutoFocus;

  LaserCutter(Driver driver) : driver(driver) {
    this->nativeUnit = IN;
    if(this->driver == EPILOG_LEGEND)
      this->supportsAutoFocus = true;
    else
      this->supportsAutoFocus = false;
  }


  // init must be called exactly once per ctrl-cut livetime and it must be called before the first invocation of getInstance.
  const LaserCutter& init(const Driver driver) {
    assert(this->instance == NULL);
    this->instance = new LaserCutter(driver);
    return *this->instance;
  }

  // return the global laser cutter model
  const LaserCutter& getInstance() {
    assert(this->instance != NULL);
    return *this->instance;
  }
};

#endif /* LASERCUTTER_H_ */

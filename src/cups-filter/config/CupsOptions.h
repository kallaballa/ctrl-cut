#ifndef CUPSOPTIONS_H_
#define CUPSOPTIONS_H_

#include <string>
#include <cups/cups.h>
#include "util/Logger.h"

using std::string;

enum CupsOptionsKey {
  DRIVER,
  BEDSIZE,
  AUTOFOCUS,
  RESOLUTION,
  RASTER_SPEED,
  RASTER_POWER,
  RASTER_DITHERING,
  RASTER_DIRECTION,
  VECTOR_SPEED,
  VECTOR_POWER,
  VECTOR_FREQUENCY,
  VECTOR_REDUCE,
  VECTOR_OPTIMIZE,
  ENABLE_DEBUG,
  ENABLE_RASTER,
  ENABLE_VECTOR
};
class CupsOptions {
private:
  cups_option_t *options;
  int numOptions;
  static const string OPTION_STRINGS[];
  static const size_t OPTION_STRINGS_SIZE;
public:
  CupsOptions(cups_option_t *options, int numOptions) : options(options), numOptions(numOptions) {};
  virtual ~CupsOptions(){};

  const bool get(CupsOptionsKey key, string& v) {
    return get(this->OPTION_STRINGS[key], v);
  }

  const bool get(const string& key, string& v) {
    const char* cupsV = cupsGetOption(key.c_str(), this->numOptions, this->options);

    if(cupsV == NULL)
      return false;

    v = cupsV;
    return true;
  }

  void dump() {
    for(size_t i = 0; i < OPTION_STRINGS_SIZE; i++) {
      string v;
      if(get(OPTION_STRINGS[i],v))
        LOG_INFO_MSG(OPTION_STRINGS[i], v);
    }
  }
};

#endif /* CUPSOPTIONS_H_ */

static const size_t OPTION_STRINGS_SIZE=15;
const string CupsOptions::OPTION_STRINGS[]= {
    "Driver",
    "BedSize",
    "AutoFocus",
    "Resolution",
    "RasterSpeed",
    "RasterPower",
    "RasterDithering",
    "RasterDirection",
    "VectorSpeed",
    "VectorPower",
    "VectorFrequency",
    "VectorReduce",
    "VectorOptimze",
    "Debug",
    "EnableRaster",
    "EnableVector"
  };

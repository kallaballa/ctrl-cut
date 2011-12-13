#ifndef CUPSOPTIONS_H_
#define CUPSOPTIONS_H_

#include <string>
#include <cups/cups.h>
#include <boost/lexical_cast.hpp>
#include "util/Logger.h"
#include "config/DocumentSettings.h"
#include "config/EngraveSettings.h"
#include "config/CutSettings.h"
#include "config/LaserCutter.h"

using std::string;
using boost::lexical_cast;

class CupsOptions {
private:
  cups_option_t *options;
  int numOptions;
  static const string OPTION_STRINGS[];
  static const size_t OPTION_STRINGS_SIZE;
public:

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

  typedef DocumentSettings DS;
  typedef CutSettings CSET;
  typedef EngraveSettings ESET;

  /*!
   Copy supported options into the supplied
   */
  void parseSettings(DocumentSettings& ds)
  {
    string v;

    if (this->get(DRIVER,v)) {
      if (v == "EpilogLegend") {
        ds.put(DS::DRIVER, LaserCutter::EPILOG_LEGEND);
      }
      else if (v == "EpilogZing") {
        ds.put(DS::DRIVER, LaserCutter::EPILOG_ZING);
      }
      else {
        LOG_WARN_MSG("Illegal value for Driver", v);
      }
    }
    if (this->get(BEDSIZE,v)) {
      if (v == "16x12") {
        ds.put(DS::WIDTH, Measurement(16, IN));
        ds.put(DS::HEIGHT, Measurement(12, IN));
      }
      else if (v == "24x12") {
        ds.put(DS::WIDTH, Measurement(24, IN));
        ds.put(DS::HEIGHT, Measurement(12, IN));
      }
      else if (v == "36x24") {
        ds.put(DS::WIDTH, Measurement(36, IN));
        ds.put(DS::HEIGHT, Measurement(24, IN));
      }
      else {
        LOG_WARN_MSG("Illegal value for BedSize", v);
      }
    }

    if (this->get(AUTOFOCUS,v)) {
      ds.put(DS::AUTO_FOCUS, (v != "false"));
    }
    if (this->get(RESOLUTION,v)) {
      ds.put(DS::RESOLUTION, lexical_cast<uint16_t>(v));
    }
    if (this->get(RASTER_SPEED,v)) {
      ds.put(ESET::ESPEED, lexical_cast<uint16_t>(v));
    }
    if (this->get(RASTER_POWER,v)) {
      ds.put(ESET::EPOWER, lexical_cast<uint16_t>(v));
    }
    if (this->get(RASTER_DITHERING,v)) {
      if (v == "Default") ds.put(ESET::DITHERING, ESET::DEFAULT_DITHERING);
      else if (v == "Bayer") ds.put(ESET::DITHERING, ESET::BAYER);
      else if (v == "FloydSteinberg") ds.put(ESET::DITHERING, ESET::FLOYD_STEINBERG);
      else if (v == "Jarvis") ds.put(ESET::DITHERING, ESET::JARVIS);
      else if (v == "Burke") ds.put(ESET::DITHERING, ESET::BURKE);
      else if (v == "Stucki") ds.put(ESET::DITHERING, ESET::STUCKI);
      else if (v == "Sierra2") ds.put(ESET::DITHERING, ESET::SIERRA2);
      else if (v == "Sierra3") ds.put(ESET::DITHERING, ESET::SIERRA3);
      else {
        LOG_WARN_MSG("Illegal value for RasterDithering", v);
      }
    }
    if (this->get(RASTER_DIRECTION,v)) {
      if (v == "TopDown")
        ds.put(ESET::DIRECTION, ESET::TOPDOWN);
      else if (v == "BottomUp")
        ds.put(ESET::DIRECTION, ESET::BOTTOMUP);
      else {
        LOG_WARN_MSG("Illegal value for RasterDirection", v);
      }
    }
    if (this->get(VECTOR_SPEED,v)) {
      ds.put(CSET::CSPEED, lexical_cast<uint16_t>(v));
    }
    if (this->get(VECTOR_POWER,v)) {
      ds.put(CSET::CPOWER, lexical_cast<uint16_t>(v));
    }
    if (this->get(VECTOR_FREQUENCY,v)) {
      ds.put(CSET::FREQUENCY, lexical_cast<uint16_t>(v));
    }
    if (this->get(VECTOR_REDUCE,v)) {
      ds.put(CSET::REDUCE, lexical_cast<uint16_t>(v));
    }
    if (this->get(VECTOR_OPTIMIZE,v)) {
      if (v == "Simple") ds.put(CSET::OPTIMIZE, CSET::SIMPLE);
      else if (v == "Inner-Outer") ds.put(CSET::OPTIMIZE, CSET::INNER_OUTER);
      else if (v == "Shortest-Path") ds.put(CSET::OPTIMIZE, CSET::SHORTEST_PATH);
      else {
        LOG_WARN_MSG("Illegal value for VectorOptimize", v);
      }
    }
    if (this->get(ENABLE_DEBUG,v)) {
      if(v != "false")
        Logger::init(CC_DEBUG);
    }
    if (this->get(ENABLE_RASTER,v)) {
      ds.put(DS::ENABLE_RASTER, v != "false");
    }
    if (this->get(ENABLE_VECTOR,v)) {
      ds.put(DS::ENABLE_VECTOR, v != "false");
    }

    if (ds.get(DS::DRIVER) == LaserCutter::UNINITIALIZED) {
      LOG_FATAL_STR("Driver not specified.");
    }

    if (ds.get(DS::RESOLUTION)  <= 0) {
      LOG_FATAL_STR("Resolution not specified.");
    }

    ds.log();
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

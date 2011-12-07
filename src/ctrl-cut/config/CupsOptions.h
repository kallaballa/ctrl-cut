#ifndef CUPSOPTIONS_H_
#define CUPSOPTIONS_H_

#include <string>
#include <cups/cups.h>
#include <boost/lexical_cast.hpp>
#include "util/Logger.h"
#include "DocumentSettings.h"
#include "EngraveSettings.h"
#include "CutSettings.h"
#include "LaserCutter.h"

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


  /*!
   Copy supported options into the supplied
   */
  void parseSettings(DocumentSettings& ds)
  {
    string v;
    typedef DocumentSettings D_SET;
    typedef CutSettings C_SET;
    typedef EngraveSettings E_SET;

    if (this->get(DRIVER,v)) {
      if (v == "EpilogLegend") {
        ds[D_SET::DRIVER] = LaserCutter::EPILOG_LEGEND;
      }
      else if (v == "EpilogZing") {
        ds[D_SET::DRIVER] = LaserCutter::EPILOG_ZING;
      }
      else {
        LOG_WARN_MSG("Illegal value for Driver", v);
      }
    }
    if (this->get(BEDSIZE,v)) {
      if (v == "16x12") {
        ds[D_SET::WIDTH] = Measurement(16, IN);
        ds[D_SET::HEIGHT] = Measurement(12, IN);
      }
      else if (v == "24x12") {
        ds[D_SET::WIDTH] = Measurement(24, IN);
        ds[D_SET::HEIGHT] = Measurement(12, IN);
      }
      else if (v == "36x24") {
        ds[D_SET::WIDTH] = Measurement(36, IN);
        ds[D_SET::HEIGHT] = Measurement(24, IN);
      }
      else {
        LOG_WARN_MSG("Illegal value for BedSize", v);
      }
    }

    if (this->get(AUTOFOCUS,v)) {
      ds[D_SET::AUTO_FOCUS] = (v != "false");
    }
    if (this->get(RESOLUTION,v)) {
      ds[D_SET::RESOLUTION] = lexical_cast<uint16_t>(v);
    }
    if (this->get(RASTER_SPEED,v)) {
      ds[E_SET::ESPEED] = lexical_cast<uint16_t>(v);
    }
    if (this->get(RASTER_POWER,v)) {
      ds[E_SET::EPOWER] = lexical_cast<uint16_t>(v);
    }
    if (this->get(RASTER_DITHERING,v)) {

      if (v == "Default") ds[E_SET::DITHERING] = E_SET::DEFAULT_DITHERING;
      else if (v == "Bayer") ds[E_SET::DITHERING] = E_SET::BAYER;
      else if (v == "FloydSteinberg") ds[E_SET::DITHERING] = E_SET::FLOYD_STEINBERG;
      else if (v == "Jarvis") ds[E_SET::DITHERING] = E_SET::JARVIS;
      else if (v == "Burke") ds[E_SET::DITHERING] = E_SET::BURKE;
      else if (v == "Stucki") ds[E_SET::DITHERING] = E_SET::STUCKI;
      else if (v == "Sierra2") ds[E_SET::DITHERING] = E_SET::SIERRA2;
      else if (v == "Sierra3") ds[E_SET::DITHERING] = E_SET::SIERRA3;
      else {
        LOG_WARN_MSG("Illegal value for RasterDithering", v);
      }
    }
    if (this->get(RASTER_DIRECTION,v)) {
      if (v == "TopDown")
        ds[E_SET::DIRECTION] = E_SET::TOPDOWN;
      else if (v == "BottomUp")
        ds[E_SET::DIRECTION] = E_SET::BOTTOMUP;
      else {
        LOG_WARN_MSG("Illegal value for RasterDirection", v);
      }
    }
    if (this->get(VECTOR_SPEED,v)) {
      ds[C_SET::CSPEED] = lexical_cast<uint16_t>(v);
    }
    if (this->get(VECTOR_POWER,v)) {
      ds[C_SET::CPOWER] = lexical_cast<uint16_t>(v);
    }
    if (this->get(VECTOR_FREQUENCY,v)) {
      ds[C_SET::FREQUENCY] = lexical_cast<uint16_t>(v);
    }
    if (this->get(VECTOR_REDUCE,v)) {
      ds[C_SET::REDUCE] = lexical_cast<uint16_t>(v);
    }
    if (this->get(VECTOR_OPTIMIZE,v)) {
      if (v == "Simple") ds[C_SET::OPTIMIZE] = C_SET::SIMPLE;
      else if (v == "Inner-Outer") ds[C_SET::OPTIMIZE] = C_SET::INNER_OUTER;
      else if (v == "Shortest-Path") ds[C_SET::OPTIMIZE] = C_SET::SHORTEST_PATH;
      else {
        LOG_WARN_MSG("Illegal value for VectorOptimize", v);
      }
    }
    if (this->get(ENABLE_DEBUG,v)) {
      cc_loglevel = v != "false" ? CC_DEBUG : (LogLevel)DEBUG;
    }
    if (this->get(ENABLE_RASTER,v)) {
      ds[D_SET::ENABLE_RASTER] = v != "false";
    }
    if (this->get(ENABLE_VECTOR,v)) {
      ds[D_SET::ENABLE_VECTOR] = v != "false";
    }

    if (ds.get<LaserCutter::Driver>(D_SET::DRIVER) == LaserCutter::UNINITIALIZED) {
      LOG_FATAL_STR("Driver not specified.");
    }

    if (ds.get<uint16_t>(D_SET::RESOLUTION)  <= 0) {
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

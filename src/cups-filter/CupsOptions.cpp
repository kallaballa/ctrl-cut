#include "CupsOptions.h"

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

typedef DocumentSettings DS;
typedef CutSettings CSET;
typedef EngraveSettings ESET;

/*!
 Copy supported options into the supplied DocumentSettings
 */
void CupsOptions::parseSettings(DocumentSettings& ds, cups_option_t *options, int numOptions) {
  string v;
  CupsOptions cupsOpts(options, numOptions);

  if (cupsOpts.get(CupsOptions::DRIVER, v)) {
    if (v == "EpilogLegend") {
      ds.put(DS::DRIVER, LaserCutter::EPILOG_LEGEND);
    } else if (v == "EpilogZing") {
      ds.put(DS::DRIVER, LaserCutter::EPILOG_ZING);
    } else {
      LOG_WARN_MSG("Illegal value for Driver", v);
    }
  }
  if (cupsOpts.get(CupsOptions::BEDSIZE, v)) {
    if (v == "16x12") {
      ds.put(DS::WIDTH, Measurement(16, IN));
      ds.put(DS::HEIGHT, Measurement(12, IN));
    } else if (v == "24x12") {
      ds.put(DS::WIDTH, Measurement(24, IN));
      ds.put(DS::HEIGHT, Measurement(12, IN));
    } else if (v == "36x24") {
      ds.put(DS::WIDTH, Measurement(36, IN));
      ds.put(DS::HEIGHT, Measurement(24, IN));
    } else {
      LOG_WARN_MSG("Illegal value for BedSize", v);
    }
  }

  if (cupsOpts.get(CupsOptions::AUTOFOCUS, v)) {
    ds.put(DS::AUTO_FOCUS, (v != "false"));
  }
  if (cupsOpts.get(CupsOptions::RESOLUTION, v)) {
    ds.put(DS::RESOLUTION, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::RASTER_SPEED, v)) {
    ds.put(ESET::ESPEED, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::RASTER_POWER, v)) {
    ds.put(ESET::EPOWER, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::RASTER_DITHERING, v)) {
    if (v == "Default")
      ds.put(ESET::DITHERING, ESET::DEFAULT_DITHERING);
    else if (v == "Bayer")
      ds.put(ESET::DITHERING, ESET::BAYER);
    else if (v == "FloydSteinberg")
      ds.put(ESET::DITHERING, ESET::FLOYD_STEINBERG);
    else if (v == "Jarvis")
      ds.put(ESET::DITHERING, ESET::JARVIS);
    else if (v == "Burke")
      ds.put(ESET::DITHERING, ESET::BURKE);
    else if (v == "Stucki")
      ds.put(ESET::DITHERING, ESET::STUCKI);
    else if (v == "Sierra2")
      ds.put(ESET::DITHERING, ESET::SIERRA2);
    else if (v == "Sierra3")
      ds.put(ESET::DITHERING, ESET::SIERRA3);
    else {
      LOG_WARN_MSG("Illegal value for RasterDithering", v);
    }
  }
  if (cupsOpts.get(CupsOptions::RASTER_DIRECTION, v)) {
    if (v == "TopDown")
      ds.put(ESET::DIRECTION, ESET::TOPDOWN);
    else if (v == "BottomUp")
      ds.put(ESET::DIRECTION, ESET::BOTTOMUP);
    else {
      LOG_WARN_MSG("Illegal value for RasterDirection", v);
    }
  }
  if (cupsOpts.get(CupsOptions::VECTOR_SPEED, v)) {
    ds.put(CSET::CSPEED, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::VECTOR_POWER, v)) {
    ds.put(CSET::CPOWER, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::VECTOR_FREQUENCY, v)) {
    ds.put(CSET::FREQUENCY, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::VECTOR_REDUCE, v)) {
    ds.put(CSET::REDUCE, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::VECTOR_OPTIMIZE, v)) {
    if (v == "Simple")
      ds.put(CSET::OPTIMIZE, CSET::SIMPLE);
    else if (v == "Inner-Outer")
      ds.put(CSET::OPTIMIZE, CSET::INNER_OUTER);
    else if (v == "Shortest-Path")
      ds.put(CSET::OPTIMIZE, CSET::SHORTEST_PATH);
    else {
      LOG_WARN_MSG("Illegal value for VectorOptimize", v);
    }
  }
  if (cupsOpts.get(CupsOptions::ENABLE_DEBUG, v)) {
    if (v != "false")
      Logger::init(CC_DEBUG);
  }
  if (cupsOpts.get(CupsOptions::ENABLE_RASTER, v)) {
    ds.put(DS::ENABLE_RASTER, v != "false");
  }
  if (cupsOpts.get(CupsOptions::ENABLE_VECTOR, v)) {
    ds.put(DS::ENABLE_VECTOR, v != "false");
  }

  if (ds.get(DS::DRIVER) == LaserCutter::UNINITIALIZED) {
    LOG_FATAL_STR("Driver not specified.");
  }

  if (ds.get(DS::RESOLUTION) <= 0) {
    LOG_FATAL_STR("Resolution not specified.");
  }
}

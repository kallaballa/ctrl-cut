#include "CupsOptions.hpp"

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
    "EnableEngraving",
    "EnableVector"
  };

typedef DocumentSettings DS;
typedef CutSettings CS;
typedef EngraveSettings ES;

/*!
 Translate supported options into the supplied DocumentSettings
 */
CupsOptions CupsOptions::parseSettings(DocumentSettings& ds, cups_option_t *options, int numOptions) {
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
  if (cupsOpts.get(CupsOptions::RESOLUTION, v)) {
    ds.put(DS::RESOLUTION, lexical_cast<uint16_t> (v));
  }

  if (cupsOpts.get(CupsOptions::BEDSIZE, v)) {
    uint16_t res = ds.get(DS::RESOLUTION);
    if (v == "16x12") {
      ds.put(DS::WIDTH, Distance(16, IN, res));
      ds.put(DS::HEIGHT, Distance(12, IN, res));
    } else if (v == "24x12") {
      ds.put(DS::WIDTH, Distance(24, IN, res));
      ds.put(DS::HEIGHT, Distance(12, IN, res));
    } else if (v == "36x24") {
      ds.put(DS::WIDTH, Distance(36, IN, res));
      ds.put(DS::HEIGHT, Distance(24, IN, res));
    } else {
      LOG_WARN_MSG("Illegal value for BedSize", v);
    }
  }

  if (cupsOpts.get(CupsOptions::AUTOFOCUS, v)) {
    ds.put(DS::AUTO_FOCUS, (v != "false"));
  }

  if (cupsOpts.get(CupsOptions::RASTER_SPEED, v)) {
    ds.put(ES::ESPEED, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::RASTER_POWER, v)) {
    ds.put(ES::EPOWER, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::RASTER_DITHERING, v)) {
    if (v == "Default")
      ds.put(ES::DITHERING, ES::DEFAULT_DITHERING);
    else if (v == "Bayer")
      ds.put(ES::DITHERING, ES::BAYER);
    else if (v == "FloydSteinberg")
      ds.put(ES::DITHERING, ES::FLOYD_STEINBERG);
    else if (v == "Jarvis")
      ds.put(ES::DITHERING, ES::JARVIS);
    else if (v == "Burke")
      ds.put(ES::DITHERING, ES::BURKE);
    else if (v == "Stucki")
      ds.put(ES::DITHERING, ES::STUCKI);
    else if (v == "Sierra2")
      ds.put(ES::DITHERING, ES::SIERRA2);
    else if (v == "Sierra3")
      ds.put(ES::DITHERING, ES::SIERRA3);
    else {
      LOG_WARN_MSG("Illegal value for RasterDithering", v);
    }
  }
  if (cupsOpts.get(CupsOptions::RASTER_DIRECTION, v)) {
    if (v == "TopDown")
      ds.put(ES::DIRECTION, ES::TOPDOWN);
    else if (v == "BottomUp")
      ds.put(ES::DIRECTION, ES::BOTTOMUP);
    else {
      LOG_WARN_MSG("Illegal value for RasterDirection", v);
    }
  }
  if (cupsOpts.get(CupsOptions::VECTOR_SPEED, v)) {
    ds.put(CS::CSPEED, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::VECTOR_POWER, v)) {
    ds.put(CS::CPOWER, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::VECTOR_FREQUENCY, v)) {
    ds.put(CS::FREQUENCY, lexical_cast<uint16_t> (v));
  }
  if (cupsOpts.get(CupsOptions::VECTOR_OPTIMIZE, v)) {
    if (v == "Simple")
      ds.put(CS::SORT, CS::SIMPLE);
    else if (v == "Inner-Outer")
      ds.put(CS::SORT, CS::INNER_OUTER);
    else if (v == "Shortest-Path")
      ds.put(CS::SORT, CS::SHORTEST_PATH);
    else {
      LOG_WARN_MSG("Illegal value for VectorOptimize", v);
    }
  }
  if (cupsOpts.get(CupsOptions::ENABLE_DEBUG, v)) {
    if (v != "false")
      Logger::init(CC_DEBUG);
  }
  if (cupsOpts.get(CupsOptions::ENABLE_ENGRAVING, v)) {
    ds.put(DS::ENABLE_ENGRAVING, v != "false");
  }

  if (cupsOpts.get(CupsOptions::ENABLE_VECTOR, v)) {
    ds.put(DS::ENABLE_CUT, v != "false");
  }

  if (ds.get(DS::DRIVER) == LaserCutter::UNINITIALIZED) {
    LOG_FATAL_STR("Driver not specified.");
  }

  if (ds.get(DS::RESOLUTION) <= 0) {
    LOG_FATAL_STR("Resolution not specified.");
  }

  return cupsOpts;
}

/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef DOCUMENT_SETTINGS_H_
#define DOCUMENT_SETTINGS_H_

#include "Settings.hpp"
#include "LaserCutter.hpp"

class DocumentSettings : public Settings
{
public:
  enum Center {
    CENTER_NONE,
    CENTER_CENTER,
    CENTER_LEFT,
    CENTER_RIGHT,
    CENTER_TOP,
    CENTER_BOTTOM
  };

  enum AirAssist {
    GLOBAL,
    RASTER_ONLY,
    CUT_ONLY,
    OFF
  };

  const static Key<string> DUUID;

  // the configured laser cutter hardware
  const static Key<LaserCutter::Driver> DRIVER;
  const static Key<string> USER;
  const static Key<string> TITLE;
  const static Key<string>  DATA_DIR;
  const static Key<string>  FILENAME;

  const static Key<uint16_t> RESOLUTION;
  const static Key<Distance> WIDTH;
  const static Key<Distance> HEIGHT;
  const static Key<bool> AUTO_FOCUS;
  const static Key<AirAssist> AIR_ASSIST;
  const static Key<Center> CENTER;
  const static Key<uint16_t> SCREEN;

  const static Key<bool> ENABLE_ENGRAVING;
  const static Key<bool> ENABLE_CUT;
  const static Key<bool> LOAD_CUT;
  const static Key<bool> LOAD_ENGRAVING;
  const static Key<bool> DUMP_XML;

  DocumentSettings();
  ~DocumentSettings() {}

  void toJson(std::ostream& os) const {
    os << "{" << std::endl;
    os << "\"uuid\":\"" << this->get(DocumentSettings::DUUID) << "\"," << std::endl;
    os << "\"title\":\"" << this->get(DocumentSettings::TITLE) << "\"," << std::endl;
    os << "\"width\":" << this->get(DocumentSettings::WIDTH) << "," << std::endl;
    os << "\"height\":" << this->get(DocumentSettings::HEIGHT) << "," << std::endl;
    os << "\"resolution\":" << this->get(DocumentSettings::RESOLUTION) << std::endl;
    os << "}" ;
  }


  static Center parseCenterName(const string& name) {
    if(name == "None") {
      return CENTER_NONE;
    } else if(name == "Center") {
      return CENTER_CENTER;
    } else if(name == "Left") {
        return CENTER_LEFT;
    } else if(name == "Right") {
        return CENTER_RIGHT;
    } else if(name == "Top") {
        return CENTER_TOP;
    } else if(name == "Bottom") {
        return CENTER_BOTTOM;
    }
    else assert(false);
    return CENTER_NONE;
  }

  static std::string getCenterName(Center c) {
    switch(c) {
      case CENTER_NONE:
        return "None";
      case CENTER_CENTER:
        return "Center";
      case CENTER_LEFT:
        return "Left";
      case CENTER_RIGHT:
        return "Right";
      case CENTER_TOP:
        return "Center-Top";
      case CENTER_BOTTOM:
        return "Bottom";
      default:
        return "Unknown";
    }
  }

  static AirAssist parseAirAssistName(const string& name) {
    if(name == "Global") {
      return GLOBAL;
    } else if(name == "Raster only") {
      return RASTER_ONLY;
    } else if(name == "Cut only") {
      return CUT_ONLY;
    }else if(name == "Off") {
        return OFF;
    }
    else assert(false);
    return GLOBAL;
  }

  static std::string getAirAssistName(AirAssist c) {
    switch(c) {
      case GLOBAL:
        return "Global";
      case RASTER_ONLY:
        return "Raster only";
      case CUT_ONLY:
        return "Cut only";
      case OFF:
        return "Off";
      default:
        return "Unknown";
    }
  }
};

#endif /* DOCUMENT_SETTINGS_H_ */

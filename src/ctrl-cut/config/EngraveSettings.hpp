/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef ENGRAVE_SETTINGS_H_
#define ENGRAVE_SETTINGS_H_


#include <stdint.h>
#include <string>
#include <sstream>
#include "cut/geom/Geometry.hpp"
#include "config/DocumentSettings.hpp"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

class EngraveSettings : public Settings
{
public:
  enum Dithering {
    DEFAULT_DITHERING,
    BAYER,
    FLOYD_STEINBERG,
    JARVIS,
    BURKE,
    STUCKI,
    SIERRA2,
    SIERRA3
  };

  enum Direction {
    TOPDOWN = 0,
    BOTTOMUP = 1
  };

  static Key<std::string> EUUID;
  static Key<Dithering> DITHERING;
  static Key<Direction> DIRECTION;
  static Key<Point> EPOS;
  static Key<uint16_t> EPOWER;
  static Key<uint16_t> ESPEED;

  EngraveSettings(const DocumentSettings& docSettings) : Settings(docSettings) {
    std::stringstream ss;
    ss << boost::uuids::random_generator()();
    this->put(EngraveSettings::EUUID, ss.str());

    try {
      this->put(EngraveSettings::DITHERING, docSettings.get(EngraveSettings::DITHERING));
    } catch(std::exception& ex) {
      this->put(EngraveSettings::DITHERING, EngraveSettings::FLOYD_STEINBERG);
    }

    try {
      this->put(EngraveSettings::DIRECTION, docSettings.get(EngraveSettings::DIRECTION));
    } catch(std::exception& ex) {
      this->put(EngraveSettings::DIRECTION, EngraveSettings::TOPDOWN);
    }

    try {
      this->put(EngraveSettings::EPOWER, docSettings.get(EngraveSettings::EPOWER));
    } catch(std::exception& ex) {
      this->put(EngraveSettings::EPOWER, 20);
    }

    try {
      this->put(EngraveSettings::ESPEED, docSettings.get(EngraveSettings::ESPEED));
    } catch(std::exception& ex) {
      this->put(EngraveSettings::ESPEED, 100);
    }

    try {
      this->put(EngraveSettings::EPOS, docSettings.get(EngraveSettings::EPOS));
    } catch(std::exception& ex) {
      this->put(EngraveSettings::EPOS, Point());
    }
  }

  EngraveSettings(const EngraveSettings& other) : Settings(other){
    Settings::setParent(other.parent);
  }

  ~EngraveSettings() {}

  void toJson(std::ostream& os) const {
    os << "{" << std::endl;
    os << "\"uuid\":\"" << this->get(EngraveSettings::EUUID) << "\"," << std::endl;
    os << "\"dithering\":\"" << getDitheringName(this->get(EngraveSettings::DITHERING)) << "\"," << std::endl;
    os << "\"direction\":\"" << getDirectionName(this->get(EngraveSettings::DIRECTION)) << "\"," << std::endl;
    os << "\"power\":" << this->get(EngraveSettings::EPOWER) << "," << std::endl;
    os << "\"speed\":" << this->get(EngraveSettings::ESPEED) << "," << std::endl;
    os << "\"position\":";
    this->get(EngraveSettings::EPOS).toJson(os);
    os << std::endl << "}" ;
  }

  static Dithering parseDitheringName(const string& name) {
    if(name == "Bayer") {
      return EngraveSettings::BAYER;
    } else if(name == "Floyd-Steinberg") {
        return EngraveSettings::FLOYD_STEINBERG;
    } else if(name == "Jarvis") {
        return EngraveSettings::JARVIS;
    } else if(name == "Burke") {
        return EngraveSettings::BURKE;
    } else if(name == "Stucki") {
        return EngraveSettings::STUCKI;
    } else if(name == "Sierra2") {
        return EngraveSettings::SIERRA2;
    } else if(name == "Sierra3") {
        return EngraveSettings::SIERRA3;
    }
    else assert(false);
  }

  static Direction parseDirectionName(const string& name) {
    if(name == "Top-Down") {
      return EngraveSettings::TOPDOWN;
    } else if(name == "Bottom-Up") {
      return EngraveSettings::BOTTOMUP;
    }
    else assert(false);
  }

  static std::string getDitheringName(Dithering opt) {
    switch (opt) {
    case EngraveSettings::BAYER:
      return "Bayer";
      break;
    case EngraveSettings::FLOYD_STEINBERG:
      return "Floyd-Steinberg";
      break;
    case EngraveSettings::JARVIS:
      return "Jarvis";
      break;
    case EngraveSettings::BURKE:
      return "Burke";
      break;
    case EngraveSettings::STUCKI:
      return "Stucki";
      break;
    case EngraveSettings::SIERRA2:
      return "Sierra2";
      break;
    case EngraveSettings::SIERRA3:
      return "Sierra3";
      break;
    case EngraveSettings::DEFAULT_DITHERING:
      return "Default";
      break;
    default:
      return "Unknown";
      break;
    }
  }

  static std::string getDirectionName(Direction opt) {
    switch (opt) {
    case EngraveSettings::TOPDOWN:
      return "Top-Down";
      break;
    case EngraveSettings::BOTTOMUP:
      return "Bottom-Up";
      break;
    default:
      return "Unknown";
      break;
    }
  }
};

#endif /* ENGRAVE_SETTINGS_H_ */

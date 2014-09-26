/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
    this->put(EngraveSettings::DITHERING, EngraveSettings::FLOYD_STEINBERG);
    this->put(EngraveSettings::DIRECTION, EngraveSettings::TOPDOWN);
    this->put(EngraveSettings::EPOWER, 20);
    this->put(EngraveSettings::ESPEED, 100);
    this->put(EngraveSettings::EPOS, Point());
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
  }

  static Direction parseDirectionName(const string& name) {
    if(name == "Top-Down") {
      return EngraveSettings::TOPDOWN;
    } else if(name == "Bottom-Up") {
      return EngraveSettings::BOTTOMUP;
    }
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

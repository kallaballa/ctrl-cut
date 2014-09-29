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

#ifndef CUT_SETTINGS_H_
#define CUT_SETTINGS_H_

#include <string>
#include <sstream>
#include "DocumentSettings.hpp"
#include "cut/geom/Geometry.hpp"
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators
#include <boost/uuid/uuid_io.hpp>         // streaming operators etc.

class CutSettings : public Settings
{
public:
  enum Sort {
    SIMPLE,
    INNER_OUTER,
    SHORTEST_PATH
  };

  static Key<std::string> CUUID;
  static Key<Sort> SORT;
  static Key<bool> CLIP;
  static Key<Point> CPOS;
  static Key<uint16_t> CSPEED;
  static Key<uint16_t> CPOWER;
  static Key<uint16_t> FREQUENCY;

  CutSettings() : Settings() {}
  CutSettings(DocumentSettings& docSettings) : Settings(docSettings) {
    std::stringstream ss;
    ss << boost::uuids::random_generator()();
    this->put(CutSettings::CUUID, ss.str());

    try{
      this->put(CutSettings::SORT, docSettings.get(CutSettings::SORT));
    } catch(std::exception& ex) {
      this->put(CutSettings::SORT, CutSettings::INNER_OUTER);
    }

    try {
      this->put(CutSettings::CSPEED, docSettings.get(CutSettings::CSPEED));
    } catch (std::exception& ex) {
      this->put(CutSettings::CSPEED, 33);
    }


    try {
      this->put(CutSettings::CPOWER, docSettings.get(CutSettings::CPOWER));
    } catch (std::exception& ex) {
      this->put(CutSettings::CPOWER, 80);
    }

    try {
      this->put(CutSettings::FREQUENCY, docSettings.get(CutSettings::FREQUENCY));
    } catch (std::exception& ex) {
      this->put(CutSettings::FREQUENCY, 5000);
    }

    try {
      this->put(CutSettings::CPOS, docSettings.get(CutSettings::CPOS));
    } catch (std::exception& ex) {
      this->put(CutSettings::CPOS, Point());
    }

  }
  CutSettings(const CutSettings& other) : Settings(other) {
    Settings::setParent(other.parent);
  }

  static string getSortName(Sort opt) {
    switch(opt) {
    case SIMPLE:
      return "Simple";
      break;
    case INNER_OUTER:
      return "Inner-Outer";
      break;
    case SHORTEST_PATH:
      return "Shortest Path";
      break;
    }
    return "Unkown";
  }

  static Sort parseSortName(const string& name) {
    if(name == "Simple") {
      return SIMPLE;
    } else if(name == "Inner-Outer") {
      return INNER_OUTER;
    } else if(name == "Shortest Path") {
      return SHORTEST_PATH;
    }
    else assert(false);
  }

  ~CutSettings() {}

  void toJson(std::ostream& os) const {
    os << "{" << std::endl;
    os << "\"uuid\":\"" << this->get(CutSettings::CUUID) << "\"," << std::endl;
    os << "\"sort\":\"" << getSortName(this->get(CutSettings::SORT)) << "\"," << std::endl;
    os << "\"speed\":" << this->get(CutSettings::CSPEED) << "," << std::endl;
    os << "\"power\":" << this->get(CutSettings::CPOWER) << "," << std::endl;
    os << "\"frequency\":" << this->get(CutSettings::FREQUENCY) << "," << std::endl;
    os << "\"position\":";
    this->get(CutSettings::CPOS).toJson(os);
    os << std::endl << "}" ;
  }
};

#endif /* CUT_SETTINGS_H_ */

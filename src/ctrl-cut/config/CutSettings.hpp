/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
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
    SHORTEST_PATH,
    TSP
  };

  static Key<std::string> CUUID;
  static Key<Sort> SORT;
  static Key<bool> CLIP;
  static Key<Point> CPOS;
  static Key<uint16_t> CSPEED;
  static Key<uint16_t> CPOWER;
  static Key<uint16_t> FREQUENCY;
  static Key<Distance> REDUCE;

  CutSettings() : Settings() {}
  CutSettings(DocumentSettings& docSettings) : Settings(docSettings) {
    std::stringstream ss;
    ss << boost::uuids::random_generator()();
    this->put(CutSettings::CUUID, ss.str());

    try {
      this->put(CutSettings::CPOS, docSettings.get(CutSettings::CPOS));
    } catch (std::exception& ex) {
      this->put(CutSettings::CPOS, Point());
    }
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
      this->put(CutSettings::REDUCE, docSettings.get(CutSettings::REDUCE));
    } catch (std::exception& ex) {
      this->put(CutSettings::REDUCE, Distance(0.2, MM, docSettings.get(DocumentSettings::RESOLUTION)));
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
    case TSP:
      return "TSP";

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
    } else if(name == "TSP") {
      return TSP;
    } else
      assert(false);
    return SIMPLE;
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

  virtual bool operator==(const CutSettings& other) {
    return this->properties.size() == other.properties.size()
        && std::equal(this->properties.begin(), this->properties.end(), other.properties.begin(),
            [&](const SettingsMap::value_type& one, const SettingsMap::value_type& two) {
      if(one.first == CutSettings::CPOS || one.first == CutSettings::CUUID)
        return true;

      bool result = one.first == two.first && value(one) == value(two);
      return result;
    });
  }
};

#endif /* CUT_SETTINGS_H_ */

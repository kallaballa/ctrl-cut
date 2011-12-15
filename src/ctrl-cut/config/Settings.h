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

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <iostream>
#include <string>

#include <map>
#include <set>
#include <vector>
#include <algorithm>
#include <functional>

#include <boost/any.hpp>
#include <boost/bind.hpp>

#include "util/Logger.h"

using std::string;

class Settings
{
public:
  class setting_not_found : public std::exception
  {
  public:
    const string keyid;
    setting_not_found(const string keyid) : keyid(keyid) {}
    virtual ~setting_not_found() throw () {}

  public:
      virtual const char * what() const throw() {
          return ("setting_not_found: " + keyid).c_str();
      }
  };

  template<typename T>
  class Key {
  public:
    const string id;

    Key(const char* id): id(id) {};

    operator std::string() const{
      return this->id;
    }
  };

  typedef boost::any Value;
  typedef std::map<string,Value>  PropertyMap;
  typedef PropertyMap::iterator iterator;
  typedef PropertyMap::const_iterator const_iterator;

  Settings(Settings& parent) : parent(&parent)  {};

  Settings() : parent(NULL)  {};

  ~Settings() {}

  iterator begin() { return this->properties.begin(); }
  const_iterator begin() const  { return this->properties.begin(); }
  iterator end() { return this->properties.end(); }
  const_iterator end() const  { return this->properties.end(); }

  template<typename T, typename V>
  void put(const Settings::Key<T>& key, V value) {
    properties[key] = boost::any(static_cast<T>(value));
  }

  template<typename T>
  const T get(const Settings::Key<T>& key) {
    iterator it = this->properties.find(key);
    if (it != this->end()) {
      return boost::any_cast<T>((*it).second);
    } else if(parent != NULL) {
      return parent->get(key);
    }

    boost::throw_exception(setting_not_found(key.id));
  }

private:
  PropertyMap properties;
  Settings* parent;
};

#endif /* SETTINGS_H_ */

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

#ifndef TRIGGER_H_
#define TRIGGER_H_

#include <assert.h>
#include <functional>
#include <boost/concept_check.hpp>
#include <string>
/*
 * Triggers form an unary operator which functionally checks a dynamic
 * value or even corrects it (e.g. if it is out of range).
 * A trigger must provide a name and a description and should use
 * boost concept asserts to explicitly check template types.
 */

using std::string;

template<typename T>
class Trigger: public std::unary_function<T,bool> {
public:
  const string& name;
  const string& description;

  Trigger(const string name, const string description) : name(name), description(description){}
  ~Trigger() {}

  virtual bool operator() (T& value){
    assert(false);
    return false;
  };
};

template<typename T>
class Clip: public Trigger<T> {
private:
  const T& lowerBound;
  const T& higherBound;
public:
  Clip(const T lowerBound, const T higherBound) :
    Trigger<T>("clip","checks and clips a value to a range") {
    BOOST_CONCEPT_ASSERT((boost::LessThanComparable<T>));
    this->lowerBound = lowerBound;
    this->higherBound = higherBound;
  }

  ~Clip() {}

  bool operator() (T& value) {
    if(value < lowerBound) {
      value = lowerBound;
      return true;
    } else if (value > higherBound) {
      value = higherBound;
      return true;
    }
    return false;
  }
};

#endif /* TRIGGER_H_ */

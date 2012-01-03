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


#include "SvgDocument.hpp"
#include <boost/lexical_cast.hpp>
#include <algorithm>

const Unit SvgDocument::parseUnit(const string unit) {
  stringstream sUnit;
  string::const_iterator it;
  char c = ' ';

  for (it = unit.begin(); c != 0 && it != unit.end(); it++) {
    c = *it;

    // consume whitespace
    for(;isspace(c) && it != unit.end(); it++) c = *it;

    // collect unit
    for (;isalpha(c); c = *(++it)) {
      sUnit << c;
      if(it == unit.end())
        break;
    }
  }

  string u = sUnit.str();

  transform(u.begin(), u.end(), u.begin(), lower_case);
  if (u == "in")
    return IN;
  else if (u == "mm")
    return MM;
  else
    assert(false);
}

Measurement SvgDocument::parseMeasurement(string dimension) {
  stringstream sFloat;
  stringstream sUnit;
  string::const_iterator it;
  char c = ' ';

  for (it = dimension.begin(); c != 0 && it != dimension.end(); it++) {
    c = *it;

    // consume whitespace
    for(;isspace(c) && it != dimension.end(); it++) c = *it;

    // collect float
    for (;isdigit(c) || c == '.'; c = *(++it)) {
        sFloat << c;
      if(it == dimension.end())
        break;
    }
    // consume whitespace
    for (;isspace(c) && it != dimension.end(); it++) c = *it;

    // collect unit
    for (;isalpha(c); c = *(++it)) {
      sUnit << c;
      if(it == dimension.end())
        break;
    }
  }

  string digit = sFloat.str();
  string unit = sUnit.str();
  double val = -1;
  if(digit.size() > 0) {
    val = boost::lexical_cast<double>(digit);
    if(unit.size() > 0) {
      std::transform ( unit.begin(), unit.end(), unit.begin(), lower_case );
      if(unit == "in") {
        return Measurement(val,IN);
      } else if(unit == "mm") {
        return Measurement(val,MM);
      } else
        assert(false);
    } else {
      return Measurement(val,PX);
    }
  } else
    assert(false);
}

const string SvgDocument::make_viewboxstring(const double& x, const double& y, const Measurement& w, const Measurement& h) const {
  stringstream sVB;
  sVB << " viewBox=\"" << x << " " << y << " " << w.in(PX,this->dpi) << " " <<  h.in(PX,this->dpi) << "\"";
  return sVB.str();
}

const string SvgDocument::make_attriburestring(const Attribute& attr) const {
  stringstream sAttr;
  sAttr << " " << attr.name << "=\"" << attr.value << "\"";
  return sAttr.str();
}

const string SvgDocument::make_attriburestring(const string& name, const string& value) const {
  stringstream sAttr;
  sAttr << " " << name << "=\"" << value << "\"";
  return sAttr.str();
}

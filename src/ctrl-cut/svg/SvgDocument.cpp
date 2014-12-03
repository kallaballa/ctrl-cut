/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */


#include "SvgDocument.hpp"
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include "CtrlCutException.hpp"

const double SvgDocument::SVG_DEFAULT_RES = 72;
const double SvgDocument::INKSCAPE_DEFAULT_RES = 90;

Unit SvgDocument::parseUnit(const string unit) {
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

Distance SvgDocument::parseDistance(string dimension) {
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
    for (;isalpha(c) || c == '%'; c = *(++it)) {
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
        return Distance(val,IN, this->dpi);
      } else if(unit == "mm") {
        return Distance(val,MM, this->dpi);
      } else if(unit == "px") {
        return Distance(val,PX, this->dpi);
      } else if(unit == "%") {
        return Distance(-1,PX, this->dpi);
      } else
        assert(false);
    } else {
      return Distance(val,PX, this->dpi);
    }
  } else
    assert(false);
}

Box SvgDocument::parseViewBox(string strViewbox) {
  std::vector<std::string> tokens1;
  boost::split(tokens1, strViewbox, boost::is_any_of("\""));
  if(tokens1.size() != 3)
    CtrlCutException::malformedDocument("Invalid viewBox detected");

  std::vector<std::string> tokens2;
  boost::split(tokens2, tokens1[1], boost::is_any_of(" "));
  if(tokens2.size() != 4)
    CtrlCutException::malformedDocument("Invalid viewBox detected");

  Coord_t x = boost::lexical_cast<Coord_t>(tokens2[0]);
  Coord_t y = boost::lexical_cast<Coord_t>(tokens2[1]);
  Coord_t wRealPX = boost::lexical_cast<Coord_t>(tokens2[2]);
  Coord_t hRealPX = boost::lexical_cast<Coord_t>(tokens2[3]);
  return Box(x,y, wRealPX, hRealPX);
}

const string SvgDocument::make_viewboxstring(const Box& box) const {
  stringstream sVB;
  sVB << " viewBox=\"" << box.min_corner.x << " " << box.min_corner.y << " " << box.max_corner.x << " " <<  box.max_corner.y << "\"";
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

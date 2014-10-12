

#include "SvgDocument.hpp"
#include <boost/lexical_cast.hpp>
#include <algorithm>

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
        return Distance(val,IN, this->dpi);
      } else if(unit == "mm") {
        return Distance(val,MM, this->dpi);
      } else if(unit == "px")
        return Distance(val,PX, this->dpi);
      else
        assert(false);
    } else {
      return Distance(val,PX, this->dpi);
    }
  } else
    assert(false);
}

const string SvgDocument::make_viewboxstring(const double& x, const double& y, const Distance& w, const Distance& h) const {
  stringstream sVB;
  sVB << " viewBox=\"" << x << " " << y << " " << w.in(PX) << " " <<  h.in(PX) << "\"";
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

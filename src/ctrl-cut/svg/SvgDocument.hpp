/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#ifndef SVGDOCUMENT_H_
#define SVGDOCUMENT_H_

#include <stdio.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <libxml++/libxml++.h>
#include "util/Measurement.hpp"
#include "cut/geom/Geometry.hpp"

using std::getline;
using std::istream;
using std::string;
using std::stringstream;
using std::pair;
using xmlpp::SaxParser;

class SvgDocument {
private:
  static int lower_case (int c) {
    return tolower (c);
  }
public:
  typedef SaxParser::Attribute Attribute;
  typedef SaxParser::AttributeList AttributeList;

  const static double SVG_DEFAULT_RES;
  const static double INKSCAPE_DEFAULT_RES;

  Distance x;
  Distance y;
  Distance width;
  Distance height;
  Box viewbox;
  double dpi;

  Unit parseUnit(const string unit);
  Distance parseDistance(string dimension);
  Box parseViewBox(string strViewbox);
  const string make_viewboxstring(const Box& box) const;
  const string make_attriburestring(const Attribute& attr) const;
  const string make_attriburestring(const string& name, const string& value) const;

  SvgDocument() : width(*(new Distance(-1, PX, SVG_DEFAULT_RES))) , height(*(new Distance(-1, PX, SVG_DEFAULT_RES))), dpi(SVG_DEFAULT_RES) { }
  virtual ~SvgDocument(){}
};

#endif /* SVGDOCUMENT_H_ */

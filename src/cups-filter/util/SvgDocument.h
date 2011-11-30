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

#ifndef SVGDOCUMENT_H_
#define SVGDOCUMENT_H_

#include <stdio.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <libxml++/libxml++.h>

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

  enum Unit {
    PX, MM, IN
  };

  typedef pair<double, Unit> Dimension;

  const static double SVG_DEFAULT_RES = 72;
  const static double INKSCAPE_DEFAULT_RES = 90;
  const static double MM_TO_INCH = 25.4;

  Dimension width;
  Dimension height;
  double resolution;

  const Unit parseUnit(const string unit) const;
  Dimension parseDimension(string dimension) const;
  const double convert(Dimension d, Unit target) const;
  const string make_viewboxstring(const double& x, const double& y, const Dimension& w, const Dimension& h) const;
  const string make_attriburestring(const Attribute& attr) const;

  SvgDocument() : width(*(new Dimension(-1, PX))) , height(*(new Dimension(-1, PX))), resolution(SVG_DEFAULT_RES) { }
  virtual ~SvgDocument(){}
};

#endif /* SVGDOCUMENT_H_ */

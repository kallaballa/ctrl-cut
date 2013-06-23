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
 * GNU General Public License for more de0tails.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SVGWRITER_HPP_
#define SVGWRITER_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Document.hpp"
#include <boost/geometry/extensions/io/svg/svg_mapper.hpp>

class SvgWriter {
public:
  Coord_t width;
  Coord_t height;
  const string title;
  std::ofstream ostream;


  SvgWriter(Coord_t width, Coord_t height, const string& title, const char* filename);
  virtual ~SvgWriter();

  template<typename Tgeom>
  void write(const Tgeom& geom, const std::string& style) {
    ostream << boost::geometry::svg(geom, style) << std::endl;
  }

  void write(const Route& r, const std::string& style = "stroke:rgb(0,0,0);stroke-width:1");
  void text(const string& text, const Point& p, const string& style = "font-size=\"12\" fill=\"black\"");
  void writeCut(const Cut& cut, const string& style = "");
  void writeEngraving(const Engraving& engraving, const string& style = "");
  void writeDocument(const Document& d, const string& style = "");

private:
  void writeDocumentStart();
  void writeDocumentEnd();
};
#endif /* SVGWRITER_HPP_ */

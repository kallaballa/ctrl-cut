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

class SvgWriter {
public:
  Coord_t width;
  Coord_t height;
  Coord_t resolution;
  const string title;
  std::ostream& ostream;


  SvgWriter(Coord_t width, Coord_t height, Coord_t resolution, const string& title, const char* filename);
  SvgWriter(Coord_t width, Coord_t height, Coord_t resolution, const string& title, std::ostream& os);
 virtual ~SvgWriter();

  void write(const Point& point, const std::string& style = "stroke:rgb(0,0,0);stroke-width:1;fill:rgb(0,255,255);fill-opacity:0.5;stroke-opacity:0.8");
  void write(const Segment& seg, const std::string& style = "stroke:rgb(0,0,0);stroke-width:1;fill:rgb(255,0,255);fill-opacity:0.5;stroke-opacity:0.8");
  void write(const Path& p, const std::string& style = "stroke:rgb(255,0,0);stroke-width:10;fill-opacity:0.0;stroke-opacity:1");
  void write(const Route& r, const std::string& style = "stroke:rgb(0,0,0);stroke-width:1;fill:rgb(255,0,0);fill-opacity:0.5;stroke-opacity:0.8");
  void write(const string& text, const Point& p, const string& style = "font-size=\"12\" fill=\"purple\"");
  void write(const CutPtr cut, const string& style = "");
  void write(const EngravingPtr engraving, const string& style = "");
  void write(const Document& d, const string& style = "");

private:
  void writeDocumentStart();
  void writeDocumentEnd();
};
#endif /* SVGWRITER_HPP_ */

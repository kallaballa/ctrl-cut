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
#include <boost/geometry/extensions/io/svg/svg_mapper.hpp>
#include <boost/foreach.hpp>
class SvgWriter {
public:
  uint32_t width;
  uint32_t height;
  std::ofstream ostream;

  SvgWriter(uint32_t width, uint32_t height, const char* filename) : width(width), height(height), ostream(filename) {
    ostream << "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">" << std::endl;
    ostream << "<svg xmlns=\"http://www.w3.org/2000/svg\" xml:space=\"preserve\" ";
    ostream << "width=\"" << width << "px\" height=\"" << height << "px\" ";
    ostream << "viewbox=\"0 0 " << width << " " << height << "\">" << std::endl;
  }

  virtual ~SvgWriter() {
    ostream << "</svg>";
    ostream.close();
  }

  void write(const Route& r, const std::string& style = "stroke:rgb(0,0,0);stroke-width:1") {
    BOOST_FOREACH(const Path& path, r) {
      this->write(path, style);
    }
  }

  template<typename Tgeom>
  void write(const Tgeom& geom, const std::string& style) {
    ostream << boost::geometry::svg(geom, style) << std::endl;
  }

  void text(const string& text, const Point& p, const string& style = "font-size=\"12\" fill=\"black\"") {
    ostream << "<text x=\"" << p.x << "\" y=\"" << p.y << "\" " << style << ">" << text << "</text>";
  }
};

#endif /* SVGWRITER_HPP_ */

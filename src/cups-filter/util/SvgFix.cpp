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

#include <stdio.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <libxml++/libxml++.h>
#include <fstream>
#include "LaserConfig.h"
#include <iostream>

#include "SvgFix.h"

using std::getline;
using std::istream;
using std::string;
using std::stringstream;

namespace io = boost::iostreams;

typedef io::stream<io::file_descriptor_sink> fdostream;
typedef io::stream<io::file_descriptor_source> fdistream;
int lower_case2 (int c) {
  return tolower (c);
}

class SvgSax : public xmlpp::SaxParser
{
private:
  std::ostream &outsvg;
  bool done;

public:
  SvgSax(std::ostream &outsvg) : outsvg(outsvg), done(false) {}
  virtual ~SvgSax(){}
  bool isDone(){return done;}
protected:


  uint32_t parseDimensionPx(string dimension) {
    stringstream sDigits;
    stringstream sUnit;
    string::iterator it;
    char c = ' ';

    for (it = dimension.begin(); c != 0 && it != dimension.end(); it++) {
      c = *it;

      // consume whitespace
      for(;isspace(c) && it != dimension.end(); it++) c = *it;
      // collect digits
      for (;isdigit(c);c = *(++it)) {
        sDigits << c;
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

    string digit = sDigits.str();
    string unit = sUnit.str();
    uint32_t px = -1;
    if(digit.size() > 0) {
      if(unit.size() > 0) {
        transform ( unit.begin(), unit.end(), unit.begin(), lower_case2 );
        if(unit == "in") {
          px = boost::lexical_cast<uint32_t>(digit) * 72;
        } else
          assert(false);
      } else {
        px = boost::lexical_cast<uint32_t>(digit);
      }
    }

    return px;
  }

  virtual void on_end_element(const Glib::ustring& name) {
    outsvg << "</" << name << ">" << std::endl;
    std::cerr << "</" << name << ">" << std::endl;
  };

  virtual void on_start_element(const Glib::ustring& name,
                                const AttributeList& properties) {
    if(name == "svg") {
      outsvg << "<" << name << std::endl;
      std::cerr << "<" << name << std::endl;
      uint32_t width = -1, height = -1;
      for(AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++ ) {
        Attribute attr = *it;
        if(attr.name == "viewbox") {
          //nothing
        } else {
          if(attr.name == "width") {
            width = parseDimensionPx(attr.value);
          } else if(attr.name == "height") {
            height = parseDimensionPx(attr.value);
          }
          outsvg << " " << attr.name << "=\"" << attr.value << "\""<< std::endl;
          std::cerr << " " << attr.name << "=\"" << attr.value << "\""<< std::endl;
        }
      }
      outsvg << " viewBox=\"0 0 " << width << " " <<  height << "\"" << std::endl;
      std::cerr << " viewBox=\"0 0 " << width << " " <<  height << "\"" << std::endl;
    } else {
      outsvg << "<" << name << std::endl;
      std::cerr << "<" << name << std::endl;
      for(AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++ ) {
        Attribute attr = *it;

        outsvg << " " << attr.name << "=\"" << attr.value << "\""<< std::endl;
        std::cerr << " " << attr.name << "=\"" << attr.value << "\""<< std::endl;
      }
    }
    outsvg << ">" << std::endl;
    std::cerr << ">" << std::endl;

    done=true;
  };
};
void fixResolutionByViewBox(std::istream& in, std::ostream& out) {
  string line;
  SvgSax parser(out);
  parser.set_substitute_entities(true);
  parser.parse_stream(in);
}

void SvgFix::work() {
  fdistream in(fdIn,true);
  fdostream out(fdOut,true);

  fixResolutionByViewBox(in,out);
}

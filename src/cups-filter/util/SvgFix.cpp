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
#include <string>
#include <iostream>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <libxml++/libxml++.h>
#include <fstream>
#include <iostream>

#include "SvgFix.h"

using std::getline;
using std::istream;
using std::string;
using std::stringstream;

namespace io = boost::iostreams;

typedef io::stream<io::file_descriptor_sink> fdostream;
typedef io::stream<io::file_descriptor_source> fdistream;

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
  //overrides:

  virtual void on_end_element(const Glib::ustring& name) {
    outsvg << "</" << name << ">" << std::endl;
    std::cerr << "</" << name << ">" << std::endl;
  };

  virtual void on_start_element(const Glib::ustring& name,
                                const AttributeList& properties) {
    if(name == "svg") {
      outsvg << "<" << name << std::endl;
      std::cerr << "<" << name << std::endl;
      for(AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++ ) {
        Attribute attr = *it;
        if(attr.name == "viewbox") {
          //nothing
        } else {
          if(attr.name == "width") {
          //  width = boost::lexical_cast<int64_t>(attr.value);
          } else if(attr.name == "height") {
          //  height = boost::lexical_cast<int64_t>(attr.value);
          }
          outsvg << " " << attr.name << "=\"" << attr.value << "\""<< std::endl;
          std::cerr << " " << attr.name << "=\"" << attr.value << "\""<< std::endl;
        }
      }
      outsvg << " viewbox=\"0 0 2592 1728\"" << std::endl;
      std::cerr << " viewbox=\"0 0 2592 1728\"" << std::endl;
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

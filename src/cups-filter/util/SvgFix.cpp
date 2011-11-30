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
using std::pair;

namespace io = boost::iostreams;

typedef io::stream<io::file_descriptor_sink> fdostream;
typedef io::stream<io::file_descriptor_source> fdistream;


class SvgSax : public xmlpp::SaxParser
{
private:
  enum Unit{
    PX, MM, IN
  };
  typedef pair<double, Unit> Dimension;

  std::ostream &outsvg;
  Dimension &docWidth;
  Dimension &docHeight;
  uint32_t docRes;
public:
  SvgSax(std::ostream &outsvg) : outsvg(outsvg), docWidth(*(new Dimension(-1, PX))) , docHeight(*(new Dimension(-1, PX))), docRes(90){}
  virtual ~SvgSax(){}

protected:
  static int lower_case (int c) {
    return tolower (c);
  }

  const Unit parseUnit(const string unit) const {
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

  Dimension parseDimension(string dimension) const {
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
        transform ( unit.begin(), unit.end(), unit.begin(), lower_case );
        if(unit == "in") {
          return std::make_pair(val,IN);
        } else if(unit == "mm") {
          return std::make_pair(val,MM);
        } else
          assert(false);
      } else {
        return std::make_pair(val,PX);
      }
    } else
      assert(false);
  }

  const double convert(Dimension d, Unit target) const {
    double& val = d.first;
    Unit& source = d.second;
    double spx = -1;
    double tpx = -1;

    if (source == PX)
      spx = val;
    else if (source == MM)
      spx = val * (docRes / 25.4);
    else if (source == IN)
      spx = val * docRes;
    else
      assert(false);

    if (target == PX)
      return spx;
    else if (target == MM)
      tpx = spx / (docRes / 25.4);
    else if (target == IN)
      tpx = spx / docRes;
    else
      assert(false);

    return tpx;
  }

  const string make_viewboxstring(const double& x, const double& y, const Dimension& w, const Dimension& h) const {
    const double wpx = convert(w,PX);
    const double hpx = convert(h,PX);
    stringstream sVB;
    sVB << " viewBox=\"" << x << " " << y << " " << wpx << " " <<  hpx << "\"" << std::endl;
    return sVB.str();
  }

  const string make_attriburestring(const Attribute& attr) const {
    stringstream sAttr;
    sAttr << " " << attr.name << "=\"" << attr.value << "\"" << std::endl;
    return sAttr.str();
  }

  const void writeSvg(string svg) const {
    outsvg << svg << std::endl;
    std::cerr << svg << std::endl;;
  }

  virtual void on_end_element(const Glib::ustring& name) {
    writeSvg("</" + name + ">");
  };

  virtual void on_start_element(const Glib::ustring& name, const AttributeList& properties) {
    if (name == "svg") {
      writeSvg("<" + name);
      string viewBox;

      for (AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++) {
        Attribute attr = *it;

        if (attr.name == "viewBox") {
          if(viewBox.empty())
            viewBox = make_attriburestring(attr);
        } else {
          if (attr.name == "width")
            this->docWidth = parseDimension(attr.value);
          else if (attr.name == "height")
            this->docHeight = parseDimension(attr.value);
          make_attriburestring(attr);
          outsvg << " " << attr.name << "=\"" << attr.value << "\"" << std::endl;
          std::cerr << " " << attr.name << "=\"" << attr.value << "\"" << std::endl;
        }
      }

      if(viewBox.empty())
        viewBox = make_viewboxstring(0, 0, this->docWidth, this->docHeight);

      writeSvg(viewBox);
    } else {
      writeSvg("<" + name);
      for (AttributeList::const_iterator it = properties.begin(); it
          != properties.end(); it++) {
        Attribute attr = *it;
        writeSvg(make_attriburestring(attr));
      }
    }
    writeSvg(">");
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

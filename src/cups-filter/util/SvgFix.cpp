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
#include "SvgFix.h"

namespace io = boost::iostreams;

typedef io::stream<io::file_descriptor_sink> fdostream;
typedef io::stream<io::file_descriptor_source> fdistream;

class SvgSax : public SaxParser
{
private:
  SvgFix& svgFix;
  std::ostream &outsvg;
public:
  SvgSax(SvgFix& svgFix, std::ostream &outsvg) : svgFix(svgFix), outsvg(outsvg){}
  virtual ~SvgSax(){}

protected:
  virtual void on_characters(const Glib::ustring& characters) {
    outsvg << characters;
  }

  virtual void on_comment(const Glib::ustring& text){
    svgFix.findGenerator(text);
  }

  virtual void on_cdata_block(const Glib::ustring& text) {
    outsvg << text;
  }

  virtual void on_end_element(const Glib::ustring& name) {
    outsvg << "</" + name + ">";
  };

  virtual void on_start_element(const Glib::ustring& name, const AttributeList& properties) {
    if (name == "svg") {
      svgFix.fixViewbox(outsvg, name, properties);
    } else {
      svgFix.dump(outsvg, name, properties);
    }
  };
};

void SvgFix::dump(std::ostream& out, const Glib::ustring& name, const SaxParser::AttributeList& properties) {
  out << "<" << name;
  for (AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++) {
    out << document.make_attriburestring(*it);
  }
  out << ">";
}

void SvgFix::findGenerator(const Glib::ustring& text) {
  if(text.find("Inkscape") != string::npos) {
    this->generator = SvgFix::Inkscape;
    this->document.resolution = SvgDocument::INKSCAPE_DEFAULT_RES;
  } else if (text.find("CorelDRAW") != string::npos) {
    this->generator = SvgFix::CorelDraw;
  }
}

void SvgFix::fixViewbox(std::ostream& out, const Glib::ustring& name, const SaxParser::AttributeList& properties) {
  out << "<" << name;

  string viewBox;
  for (AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++) {
    Attribute attr = *it;

    if (viewBox.empty() && attr.name == "viewBox") {
        viewBox = document.make_attriburestring(attr);
    } else {
      if (attr.name == "width")
        document.width = document.parseDimension(attr.value);
      else if (attr.name == "height")
        document.height = document.parseDimension(attr.value);

      out << document.make_attriburestring(attr);
    }
  }

  if(viewBox.empty())
    viewBox = document.make_viewboxstring(0, 0, document.width, document.height);

  out << viewBox << ">";
}


void SvgFix::work() {
  fdistream in(fdIn,true);
  fdostream out(fdOut,true);
  string line;
  SvgSax parser(*this,out);
  parser.set_substitute_entities(true);
  parser.parse_stream(in);
}

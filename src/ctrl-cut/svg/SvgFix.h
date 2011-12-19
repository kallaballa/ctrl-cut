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

#ifndef SVGFIX_H_
#define SVGFIX_H_

#include <stdio.h>
#include <ctype.h>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <boost/lexical_cast.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>

#include <libxml++/libxml++.h>
#include <Magick++.h>

#include "SvgDocument.h"

using std::getline;
using std::istream;
using std::string;
using std::stringstream;
using std::pair;
using std::ofstream;
using xmlpp::SaxParser;

namespace io = boost::iostreams;

class SvgFix {
private:
  typedef io::stream<io::file_descriptor_sink> fdostream;
  typedef io::stream<io::file_descriptor_source> fdistream;

  fdistream& in;
  fdostream& out;
public:
  typedef SaxParser::Attribute Attribute;
  typedef SaxParser::AttributeList AttributeList;

  enum DocGenerator {
    Unknown, Inkscape, CorelDraw
  };
  SvgDocument document;
  DocGenerator generator;
  void writeSvg(string s);
  void writeSvg(const Glib::ustring& name, const SaxParser::AttributeList& properties);
  void dumpSvg(string s);
  void dumpBase64(string s);
  void findGenerator(const Glib::ustring& text);
  void fixJpeg(const Glib::ustring& name, const AttributeList& properties);
  void fixViewbox(const Glib::ustring& name, const AttributeList& properties);
  void writeBase64Image(const string& mimetype, string& pngBase64);

  SvgFix(int fdIn, int fdOut) :
    in(*(new fdistream(fdIn,true))), out(*(new fdostream(fdOut,true))),  generator(Unknown){ }

  virtual ~SvgFix(){}
  void work();
};

#endif /* SVGFIX_H_ */

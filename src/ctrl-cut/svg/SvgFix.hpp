/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
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

#include "SvgDocument.hpp"

using std::getline;
using std::istream;
using std::string;
using std::stringstream;
using std::pair;
using std::ofstream;
using xmlpp::SaxParser;

class SvgFix {
private:
  typedef boost::iostreams::stream<boost::iostreams::file_descriptor_sink> fdostream;
  typedef boost::iostreams::stream<boost::iostreams::file_descriptor_source> fdistream;

  fdistream* in;
  fdostream* out;
public:
  typedef SaxParser::Attribute Attribute;
  typedef SaxParser::AttributeList AttributeList;

  enum DocGenerator {
    Unknown, Inkscape, CorelDraw
  };
  SvgDocument document;
  DocGenerator generator;
  void writeSvg(const string& s);
  void writeSvg(const Glib::ustring& name, const SaxParser::AttributeList& properties);
  void findGenerator(const Glib::ustring& text);
  void fixJpeg(const Glib::ustring& name, const AttributeList& properties);
  void fixViewbox(const Glib::ustring& name, const AttributeList& properties);
  void writeBase64Image(const string& mimetype, string& pngBase64);

  SvgFix(int fdIn, int fdOut) :
    in(new fdistream(fdIn,true)), out(new fdostream(fdOut,true)),  generator(Unknown){ }

  void work();
};

#endif /* SVGFIX_H_ */

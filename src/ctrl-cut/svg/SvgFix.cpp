/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "cut/geom/Geometry.hpp"
#include "CtrlCutException.hpp"
#include "util/2D.hpp"
#include "SvgFix.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

class SvgSax : public SaxParser
{
private:
  SvgFix& svgFix;
public:
  SvgSax(SvgFix& svgFix) : svgFix(svgFix){}
  virtual ~SvgSax(){}

protected:
  virtual void on_characters(const Glib::ustring& characters) {
    svgFix.writeSvg(characters);
  }

  virtual void on_comment(const Glib::ustring& text){
    svgFix.findGenerator(text);
  }

  virtual void on_cdata_block(const Glib::ustring& text) {
    svgFix.writeSvg(text);
  }

  virtual void on_end_element(const Glib::ustring& name) {
    svgFix.writeSvg("</" + name + ">");
  };

  virtual void on_start_element(const Glib::ustring& name, const AttributeList& properties) {
    if (name == "svg") {
      svgFix.findInkscapeVersion(name, properties);
      svgFix.fixViewbox(name, properties);
    } else if (name == "image") {
      svgFix.fixJpeg(name, properties);
    } else {
      svgFix.writeSvg(name, properties);
    }
  };
};


void SvgFix::writeSvg(const string& s) {
  *out << s;
}

void SvgFix::writeSvg(const Glib::ustring& name, const SaxParser::AttributeList& properties) {
  writeSvg("<" + name);
  for (AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++) {
    writeSvg(document.make_attriburestring(*it));
  }
  writeSvg(">");
}


void SvgFix::findGenerator(const Glib::ustring& text) {
  if(text.find("Created with Inkscape") != string::npos) {
    this->generator = SvgFix::Inkscape;
    // Inkscape uses 90dpi as default resolution for the coordinates
    // in the svg but doesn't use a viewbox to make it scale correctly.
    // -> set the resolution to 90dpi so that the viewbox will correct the scale.

    this->document.dpi = SvgDocument::INKSCAPE_DEFAULT_RES;
  } else if (text.find("CorelDRAW") != string::npos) {
    this->generator = SvgFix::CorelDraw;
  }
}

void SvgFix::writeBase64Image(const string& mimetype, string& pngBase64) {
//  replace ( pngBase64.begin(), pngBase64.end(), ' ', '\n');
  const string& imgData = "data:" + mimetype + ";base64," + pngBase64;
  writeSvg(document.make_attriburestring("xlink:href", imgData));
}

// rsvg doesn't support embedded jpeg images, so we're converting them on the fly to png
void SvgFix::fixJpeg(const Glib::ustring& name, const SaxParser::AttributeList& properties) {
  writeSvg("<" + name);
  const string jpegbase64Sig="data:image/jpeg;base64,";
  const size_t jbSigSize = jpegbase64Sig.size();
  const string pngbase64Sig="data:image/png;base64,";

  for (AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++) {
    Attribute attr = *it;

    if (attr.name == "xlink:href" && attr.value.find(jpegbase64Sig.c_str(), 0, jbSigSize) != string::npos) {
      string datalink = attr.value.raw();
      string jpegBase64 = datalink.erase(0, jbSigSize);
      replace ( jpegBase64.begin(), jpegBase64.end(), ' ', '\n');
      Magick::Blob jpegblob;
      Magick::Blob pngblob;
      jpegblob.base64(jpegBase64);
      Magick::Image img(jpegblob);
      img.magick( "PNG" );
      img.write(&pngblob);
      string pngBase64 = pngblob.base64();
      writeBase64Image("image/png", pngBase64);
    } else {
      writeSvg(document.make_attriburestring(attr));
    }
  }

  writeSvg(">");
}

void SvgFix::findInkscapeVersion(const Glib::ustring& name, const SaxParser::AttributeList& properties) {
  for (AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++) {
    Attribute attr = *it;

    if (attr.name == "inkscape:version") {
      this->inkscapeVersion = attr.value;
    }
  }
}

void SvgFix::fixViewbox(const Glib::ustring& name, const SaxParser::AttributeList& properties) {
  writeSvg("<" + name);

  string viewBox;
  for (AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++) {
    Attribute attr = *it;

    if (viewBox.empty() && attr.name == "viewBox") {
        viewBox = document.make_attriburestring(attr);
    } else {
      if (attr.name == "width") {
        document.width = document.parseDistance(attr.value);
        if(this->generator != Inkscape && !this->inkscapeVersion.empty()) {
          attr.value = boost::lexical_cast<string>(document.width.in(PX)* 1.42);
        }
      }
      else if (attr.name == "height") {
        document.height = document.parseDistance(attr.value);
        if(this->generator != Inkscape && !this->inkscapeVersion.empty()) {
          attr.value = boost::lexical_cast<string>(document.height.in(PX) * 1.42);
        }
      }
      writeSvg(document.make_attriburestring(attr));
    }
  }

  // generate a viewBox in pixels reflecting the resolution
  if(generator != Inkscape && !this->inkscapeVersion.empty()) {
  /*  std::vector<std::string> tokens1;
    boost::split(tokens1, viewBox, boost::is_any_of("\""));
    if(tokens1.size() != 3)
      CtrlCutException::malformedDocument("Invalid viewBox detected");

    std::vector<std::string> tokens2;
    boost::split(tokens2, tokens1[1], boost::is_any_of(" "));
    if(tokens2.size() != 4)
      CtrlCutException::malformedDocument("Invalid viewBox detected");
    double dpiFactor = (90.0/72.0);
    Coord_t x = boost::lexical_cast<Coord_t>(tokens2[0]) * dpiFactor;
    Coord_t y = boost::lexical_cast<Coord_t>(tokens2[1]) * dpiFactor;
    Distance w = Distance(boost::lexical_cast<Coord_t>(tokens2[2]) * dpiFactor, PX, document.width.resolution);
    Distance h = Distance(boost::lexical_cast<Coord_t>(tokens2[3]) * dpiFactor, PX, document.height.resolution);

    viewBox = document.make_viewboxstring(x, y, w, h);*/

  } else if(generator == Inkscape && viewBox.empty()) {
    // inkscape uses a default resoluton of 90 dpi while rsvg assumes 72 dpi
      Distance w = Distance(document.width.in(PX) * (90.0/72.0), PX, document.width.resolution);
      Distance h = Distance(document.height.in(PX) * (90.0/72.0), PX, document.height.resolution);
      viewBox = document.make_viewboxstring(0, 0, w, h);
  } else if(viewBox.empty()){
    viewBox = document.make_viewboxstring(0, 0, document.width, document.height);
  }
  writeSvg(viewBox + ">");
}

void SvgFix::work() {
  string line;
  SvgSax parser(*this);

  parser.set_substitute_entities(true);
  parser.parse_stream(*in);

  in->close();
  out->close();
}

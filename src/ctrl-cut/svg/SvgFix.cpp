/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "cut/geom/Geometry.hpp"
#include "util/2D.hpp"
#include "SvgFix.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "util/Logger.hpp"

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
  std::cerr << s << std::endl;
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


std::vector<int> parse_inkscape_version(string v) {
  std::vector<int> result;
  std::vector<std::string> fields;
  boost::split(fields, v, boost::is_any_of(". "));
  if(fields.size() < 3) {
    LOG_WARN_MSG("Malformed inkscape version detected", v);
    while(fields.size() < 3) {
      fields.push_back("0");
    }
  }

  for(size_t i = 0; i < 3; ++i) {
    try {
      result.push_back(boost::lexical_cast<int>(fields[i]));
    } catch(std::exception& ex) {
      LOG_WARN_MSG("Malformed inkscape version detected", v);
      result.push_back(0);
    }
  }

  return result;
}

bool inkscape_version_greater(string one, string two) {
  auto v1 = parse_inkscape_version(one);
  auto v2 = parse_inkscape_version(two);

  for(size_t i = 0; i < 3; ++i) {
    if(v1[i] > v2[i])
      return true;
  }

  return false;
}

void SvgFix::fixViewbox(const Glib::ustring& name, const SaxParser::AttributeList& properties) {
  writeSvg("<" + name);

  //a document that was created with one tool and edited with inkscape
  bool inkscapeMutant = generator != Inkscape && !this->inkscapeVersion.empty();

  string strViewBox;
  for (AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++) {
    Attribute attr = *it;

    if (strViewBox.empty() && attr.name == "viewBox") {
        strViewBox = document.make_attriburestring(attr);
    } else {
      if (attr.name == "width") {
        document.width = document.parseDistance(attr.value);
        if(document.width.value != -1) {
          if(this->generator != Inkscape && !this->inkscapeVersion.empty()) {
            attr.value = boost::lexical_cast<string>(document.width.in(PX));
          }
        }
      }
      else if (attr.name == "height") {
        document.height = document.parseDistance(attr.value);
        if (document.height.value != -1) {
          if (this->generator != Inkscape && !this->inkscapeVersion.empty()) {
            attr.value = boost::lexical_cast<string>(document.height.in(PX));
          }
        }
      } else {
        writeSvg(document.make_attriburestring(attr));
      }
    }
  }

  //fabricate a viewbox
  if(!strViewBox.empty()) {
    document.viewbox = document.parseViewBox(strViewBox);
  } else if(document.width.value != -1 && document.height.value != -1) {
    document.viewbox.min_corner.x = 0;
    document.viewbox.min_corner.y = 0;
    document.viewbox.max_corner.x = document.width.in(PX);
    document.viewbox.max_corner.y = document.height.in(PX);
  } else {
    // neither an explicit viewbox nor dimensions where found. nothing to fix here.
    writeSvg(">");
    return;
  }

  // we for sure have now a viewbox, let's see if we have to fabricate dimensions
  if((document.width.value == -1 || document.height.value == -1)) {
    document.width = Distance(document.viewbox.max_corner.x, PX, document.dpi);
    document.height = Distance(document.viewbox.max_corner.y, PX, document.dpi);
  }

  // we might have to fix inkscape resolution bug
  if(generator == Inkscape && inkscape_version_greater(inkscapeVersion, "0.48.4")) {
    Distance w = Distance(document.width.in(PX) * (90.0/72.0), PX, document.width.resolution);
    Distance h = Distance(document.height.in(PX) * (90.0/72.0), PX, document.height.resolution);
    document.viewbox = Box(0, 0, w.in(PX), h.in(PX));
  }

  writeSvg(document.make_attriburestring("width", boost::lexical_cast<string>(document.width.in(PX))));
  writeSvg(document.make_attriburestring("height", boost::lexical_cast<string>(document.height.in(PX))));
  writeSvg(document.make_viewboxstring(document.viewbox) + ">");

}

void SvgFix::work() {
  string line;
  SvgSax parser(*this);

  parser.set_substitute_entities(true);
  parser.parse_stream(*in);

  in->close();
  out->close();
}

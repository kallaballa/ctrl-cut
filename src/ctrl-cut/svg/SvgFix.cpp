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

#include "SvgFix.hpp"

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
      svgFix.fixViewbox(name, properties);
    } else if (name == "image") {
      svgFix.fixJpeg(name, properties);
    } else {
      svgFix.writeSvg(name, properties);
    }
  };
};


void SvgFix::writeSvg(string s) {
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
  if(text.find("Inkscape") != string::npos) {
    this->generator = SvgFix::Inkscape;
    /* Inkscape uses 90dpi as default resolution for the coordinates
     * in the svg but doesn't use a viewbox to make it scale correctly.
     * -> set the resolution to 90dpi so that the viewbox will correct the scale.
     */
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

void SvgFix::fixViewbox(const Glib::ustring& name, const SaxParser::AttributeList& properties) {
  writeSvg("<" + name);

  string viewBox;
  for (AttributeList::const_iterator it = properties.begin(); it != properties.end(); it++) {
    Attribute attr = *it;

    if (viewBox.empty() && attr.name == "viewBox") {
        viewBox = document.make_attriburestring(attr);
    } else {
      if (attr.name == "width")
        document.width = SvgDocument::parseMeasurement(attr.value);
      else if (attr.name == "height")
        document.height = SvgDocument::parseMeasurement(attr.value);

      writeSvg(document.make_attriburestring(attr));
    }
  }

  // generate a viewBox in pixels reflecting the resolution
  if(viewBox.empty())
    viewBox = document.make_viewboxstring(0, 0, document.width, document.height);

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

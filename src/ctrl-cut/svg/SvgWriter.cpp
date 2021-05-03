/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "SvgWriter.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <Magick++.h>

SvgWriter::SvgWriter(const Route& r, std::ostream& os) :
width(r.get(DocumentSettings::WIDTH).in(PX)),
height(r.get(DocumentSettings::HEIGHT).in(PX)),
resolution(r.get(DocumentSettings::RESOLUTION)),
title(r.get(DocumentSettings::TITLE)),
autofocus(r.get(DocumentSettings::AUTO_FOCUS)),
center(r.get(DocumentSettings::CENTER)),
airAssist(r.get(DocumentSettings::AIR_ASSIST)),
ostream(os) {
  writeDocumentStart();
}

SvgWriter::SvgWriter(const Document& doc, std::ostream& os) :
width(doc.get(DocumentSettings::WIDTH).in(PX)),
height(doc.get(DocumentSettings::HEIGHT).in(PX)),
resolution(doc.get(DocumentSettings::RESOLUTION)),
title(doc.get(DocumentSettings::TITLE)),
autofocus(doc.get(DocumentSettings::AUTO_FOCUS)),
center(doc.get(DocumentSettings::CENTER)),
airAssist(doc.get(DocumentSettings::AIR_ASSIST)),
ostream(os) {
  writeDocumentStart();
}

SvgWriter::~SvgWriter() {
  this->writeDocumentEnd();
}

void SvgWriter::writeDocumentStart() {
  using namespace boost;

  ostream << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << std::endl;
  ostream << "<!-- Created with Ctrl-Cut -->" << std::endl;

  string version="1.0";
  string dtd = "http://tbd/tbd.dtd";

  format svgtag(string("<svg xmlns:dc=\"http://purl.org/dc/elements/1.1/\" ") +
      "xmlns:cc=\"http://creativecommons.org/ns#\" " +
      "xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\" " +
      "xmlns:svg=\"http://www.w3.org/2000/svg\" " +
      "xmlns:xlink=\"http://www.w3.org/1999/xlink\" " +
      "xmlns=\"http://www.w3.org/2000/svg\" " +
      "xmlns:ctrlcut=\"%s\" " +
      "width=\"%f\" " +
      "height=\"%f\" " +
      "resolution=\"%f\" " +
      "version=\"1.1\" " +
      "ctrlcut:version=\"%s\" " +
      "viewbox=\"0 0 %d %d\" >");

  format bedborder(string("<rect ") +
      "width=\"%f\" " +
      "height=\"%f\" " +
  "style=\"fill:rgb(250,250,250); stroke-width:10; stroke:rgb(32,32,32);\" />");

  format ctrlcutDoc = format("<ctrlcut:document title=\"%s\" autoFocus=\"%s\" center=\"%s\" airAssist=\"%s\"/>");

  string metadata = string("<metadata id=\"ccmetadata\">") +
      "<rdf:RDF>" +
        "<cc:Work rdf:about=\"\">" +
          "<dc:format>image/svg+xml</dc:format>" +
          "<dc:type rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />" +
          "<dc:title></dc:title>" +
        "</cc:Work>" +
      "</rdf:RDF>" +
    "</metadata>";

  ostream << (svgtag % dtd % width % height % resolution % version % width % height) << std::endl;
  ostream << (ctrlcutDoc % title % (autofocus ? "true" : "false") % DocumentSettings::getCenterName(center) % DocumentSettings::getAirAssistName(airAssist)) << std::endl;
  ostream << metadata << std::endl;
  ostream << (bedborder % width % height) << std::endl;
}

void SvgWriter::writeDocumentEnd() {
  ostream << "</svg>";
}

void SvgWriter::write(const CutPtr cut, const string& style) {
  typedef CutSettings CS;
  boost::format layer = boost::format(string("<g ctrlcut:type=\"cut\" ")
      + "ctrlcut:speed=\"%d\" "
      + "ctrlcut:power=\"%d\" "
      + "ctrlcut:frequency=\"%d\" "
      + "ctrlcut:sort=\"%d\" "
      + "transform=\"translate(%d, %d)\" >");

  Point translate = cut->get(CS::CPOS);
  ostream << layer
      % cut->get(CS::CSPEED)
      % cut->get(CS::CPOWER)
      % cut->get(CS::FREQUENCY)
      % cut->get(CS::SORT)
      % std::round(translate.x) % std::round(translate.y)
      << std::endl;

  write(static_cast<const Route&>(*cut));
  ostream << "</g>" << std::endl;
}

void SvgWriter::write(const EngravingPtr engraving, const string& style) {
  typedef EngraveSettings ES;
  boost::format layer = boost::format(string("<g ctrlcut:type=\"engraving\" ")
      + "ctrlcut:speed=\"%d\" "
      + "ctrlcut:power=\"%d\" "
      + "ctrlcut:direction=\"%d\" "
      + "ctrlcut:dithering=\"%d\" "
      + "transform=\"translate(%d, %d)\" >");
  Point translate = engraving->get(ES::EPOS);
  ostream << layer
      % engraving->get(ES::ESPEED)
      % engraving->get(ES::EPOWER)
      % engraving->get(ES::DIRECTION)
      % engraving->get(ES::DITHERING)
      % std::round(translate.x) % std::round(translate.y)
      << std::endl;

  GrayscaleImage img = engraving->getImage();
  Coord_t width = img.width() - 1;
  Coord_t height = img.height() - 1;

  Magick::Blob rawblob(img.data(), (img.rowstride()) * height);
  Magick::Blob pngblob;
  Magick::Image image(rawblob, Magick::Geometry((img.rowstride()), height), 8, "GRAY");
  image.magick( "PNG" );
  image.write(&pngblob);
  const string& imgData = "data:image/png;base64," + pngblob.base64();

  ostream << "<image x=\"0\" y=\"0\" width=\"" << width
      << "\" height=\"" << height
      << "\" xlink:href=\"" << imgData << "\" />";

  ostream << "</g>" << std::endl;
}

void SvgWriter::write(const Document& d, const std::string& style) {
  BOOST_FOREACH(const EngravingPtr engraving, d.engravings()) {
    this->write(engraving);
  }

  BOOST_FOREACH(const CutPtr& cut, d.cuts()) {
    this->write(cut);
  }
}

void SvgWriter::write(const Point& p, const std::string& style) {
  ostream << "<circle cx=\"" << p.x << "\" cy=\"" << p.y << "\" " << "r=\"10\" " << "style=\"" << style << "\"/>" << std::endl;
}

void SvgWriter::write(const Segment& seg, const std::string& style) {
  ostream << "<polyline points=\"";
  ostream << seg.first.x << "," << seg.first.y << " " << seg.second.x << "," << seg.second.y;
  ostream << "\" style=\"" << style << "\"/>" << std::endl;
}

void SvgWriter::write(const Path& path, const std::string& style) {
  ostream << "<polyline points=\"";
  BOOST_FOREACH(const Point& p, path) {
    ostream << p.x << "," << p.y << " ";
  }
  ostream << "\" style=\"" << style << "\"/>" << std::endl;
}

void SvgWriter::write(const Route& r, const std::string& style) {
  BOOST_FOREACH(const Path& path, r) {
    this->write(path);
  }
}

void SvgWriter::write(const string& text, const Point& p, const string& style) {
  ostream << "<text x=\"" << p.x << "\" y=\"" << p.y << "\" " << style << ">" << text << "</text>";
}

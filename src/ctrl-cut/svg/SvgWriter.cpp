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
* GNU General Public License for more de0tails.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include "SvgWriter.hpp"
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <Magick++.h>

SvgWriter::SvgWriter(Coord_t width, Coord_t height, Coord_t resolution, const string& title, const char* filename) : width(width), height(height), resolution(resolution), title(title), ostream(filename) {
  this->writeDocumentStart();
}

SvgWriter::~SvgWriter() {
  this->writeDocumentEnd();
  this->ostream.close();
}

void SvgWriter::writeDocumentStart() {
  using namespace boost;
  typedef DocumentSettings DS;

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

  format ctrlcutDoc = format("<ctrlcut:document title=\"%s\"/>");

  string metadata = string("<metadata id=\"ccmetadata\">") +
      "<rdf:RDF>" +
        "<cc:Work rdf:about=\"\">" +
          "<dc:format>image/svg+xml</dc:format>" +
          "<dc:type rdf:resource=\"http://purl.org/dc/dcmitype/StillImage\" />" +
          "<dc:title></dc:title>" +
        "</cc:Work>" +
      "</rdf:RDF>" +
    "</metadata>";

  ostream << (svgtag % dtd % this->width % this->height % this->resolution % version % this->width % this->height) << std::endl;
  ostream << (ctrlcutDoc % this->title) << std::endl;
  ostream << metadata << std::endl;
}

void SvgWriter::writeDocumentEnd() {
  ostream << "</svg>";
}

void SvgWriter::writeCut(const Cut& cut, const string& style) {
  typedef CutSettings CS;
  boost::format layer = boost::format(string("<g ctrlcut:type=\"cut\" ")
      + "ctrlcut:speed=\"%d\" "
      + "ctrlcut:power=\"%d\" "
      + "ctrlcut:frequency=\"%d\" "
      + "ctrlcut:sort=\"%d\" "
      + "transform=\"translate(%d, %d)\" >");

  Point translate = cut.get(CS::CPOS);
  ostream << layer
      % cut.get(CS::CSPEED)
      % cut.get(CS::CPOWER)
      % cut.get(CS::FREQUENCY)
      % cut.get(CS::OPTIMIZE)
      % translate.x % translate.y
      << std::endl;

  write(static_cast<const Route&>(cut), style);
  ostream << "</g>" << std::endl;
}

void SvgWriter::writeEngraving(const Engraving& engraving, const string& style) {
  typedef EngraveSettings ES;
  boost::format layer = boost::format(string("<g ctrlcut:type=\"engraving\" ")
      + "ctrlcut:speed=\"%d\" "
      + "ctrlcut:power=\"%d\" "
      + "ctrlcut:direction=\"%d\" "
      + "ctrlcut:dithering=\"%d\" "
      + "transform=\"translate(%d, %d)\" >");
  Point translate = engraving.get(ES::EPOS);
  ostream << layer
      % engraving.get(ES::ESPEED)
      % engraving.get(ES::EPOWER)
      % engraving.get(ES::DIRECTION)
      % engraving.get(ES::DITHERING)
      % translate.x % translate.y
      << std::endl;

  GrayscaleImage img = engraving.getImage();
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

void SvgWriter::writeDocument(const Document& d, const std::string& style) {
  BOOST_FOREACH(const Engraving* engraving, d.engravings()) {
    this->writeEngraving(*engraving, style);
  }

  BOOST_FOREACH(const Cut* cut, d.cuts()) {
    this->writeCut(*cut, style);
  }
}

void SvgWriter::write(const Route& r, const std::string& style) {
  BOOST_FOREACH(const Path& path, r) {
    this->write(path, style);
  }
}

void SvgWriter::text(const string& text, const Point& p, const string& style) {
  ostream << "<text x=\"" << p.x << "\" y=\"" << p.y << "\" " << style << ">" << text << "</text>";
}


#ifndef SVGWRITER_HPP_
#define SVGWRITER_HPP_

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "Document.hpp"

class SvgWriter {
public:
  Coord_t width;
  Coord_t height;
  Coord_t resolution;
  const string title;
  std::ostream& ostream;


  SvgWriter(Coord_t width, Coord_t height, Coord_t resolution, const string& title, const char* filename);
  SvgWriter(Coord_t width, Coord_t height, Coord_t resolution, const string& title, std::ostream& os);
 virtual ~SvgWriter();

  void write(const Point& point, const std::string& style = "stroke:rgb(0,0,0);stroke-width:1;fill:rgb(0,255,255);fill-opacity:0.5;stroke-opacity:0.8");
  void write(const Segment& seg, const std::string& style = "stroke:rgb(0,0,0);stroke-width:1;fill:rgb(255,0,255);fill-opacity:0.5;stroke-opacity:0.8");
  void write(const Path& p, const std::string& style = "stroke:rgb(255,0,0);stroke-width:10;fill-opacity:0.0;stroke-opacity:1");
  void write(const Route& r, const std::string& style = "stroke:rgb(0,0,0);stroke-width:1;fill:rgb(255,0,0);fill-opacity:0.5;stroke-opacity:0.8");
  void write(const string& text, const Point& p, const string& style = "font-size=\"12\" fill=\"purple\"");
  void write(const CutPtr cut, const string& style = "");
  void write(const EngravingPtr engraving, const string& style = "");
  void write(const Document& d, const string& style = "");

private:
  void writeDocumentStart();
  void writeDocumentEnd();
};
#endif /* SVGWRITER_HPP_ */

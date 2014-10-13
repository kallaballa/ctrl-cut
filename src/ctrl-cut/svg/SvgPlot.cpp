/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "SvgPlot.hpp"
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "svg/SvgWriter.hpp"
#include "cut/graph/SegmentGraph.hpp"
#include "cut/geom/algorithms/Algorithms.hpp"

struct hsl_color{
  double h;
  double s;
  double l;
};

struct rgb_color{
  double r;
  double g;
  double b;
};

// This is a subfunction of HSLtoRGB
void HSLtoRGB_Subfunction(uint& c, const double& temp1, const double& temp2, const double& temp3)
{
  if((temp3 * 6) < 1)
    c = (uint)((temp2 + (temp1 - temp2)*6*temp3)*100);
  else
  if((temp3 * 2) < 1)
    c = (uint)(temp1*100);
  else
  if((temp3 * 3) < 2)
    c = (uint)((temp2 + (temp1 - temp2)*(.66666 - temp3)*6)*100);
  else
    c = (uint)(temp2*100);
  return;
}

// This function converts the "color" object to the equivalent RGB values of
// the hue, saturation, and luminance passed as h, s, and l respectively
rgb_color hsl_to_rgb(hsl_color hsl)
{
  uint r = 0;
  uint g = 0;
  uint b = 0;

  hsl.l /= 100;
  hsl.s /= 100;
  hsl.h /= 360;

  if(hsl.s == 0)
  {
    r = hsl.l;
    g = hsl.l;
    b = hsl.l;
  }
  else
  {
    double temp1 = 0;
    if(hsl.l < .50)
    {
      temp1 = hsl.l*(1 + hsl.s);
    }
    else
    {
      temp1 = hsl.l + hsl.s - (hsl.l*hsl.s);
    }

    double temp2 = 2*hsl.l - temp1;

    double temp3 = 0;
    for(int i = 0 ; i < 3 ; i++)
    {
      switch(i)
      {
      case 0: // red
        {
          temp3 = hsl.h + .33333;
          if(temp3 > 1)
            temp3 -= 1;
          HSLtoRGB_Subfunction(r,temp1,temp2,temp3);
          break;
        }
      case 1: // green
        {
          temp3 = hsl.h;
          HSLtoRGB_Subfunction(g,temp1,temp2,temp3);
          break;
        }
      case 2: // blue
        {
          temp3 = hsl.h - .33333;
          if(temp3 < 0)
            temp3 += 1;
          HSLtoRGB_Subfunction(b,temp1,temp2,temp3);
          break;
        }
      default:
        {
          break;
        }
      }
    }
  }
  rgb_color rgb;
  rgb.r = ((((double)r)/100)*255);
  rgb.g = ((((double)g)/100)*255);
  rgb.b =  ((((double)b)/100)*255);
  return rgb;
}


void find_shared_points(const Route& r, std::vector<Point>& sharedPoints) {
  SegmentGraph g;

  for(const SegmentPtr seg : segments(r)) {
    g.addSegment(*seg.get());
  }

  BOOST_FOREACH(SegmentGraph::Vertex v, vertices(g)) {
    if (boost::degree(v, g) > 2) {
      sharedPoints.push_back(g[v]);
    }
  }
}

void plot_shared_segments(const Route& r, const char* filename) {
  std::set<Segment> segidx;
  std::ofstream os(filename);
  SvgWriter svg(r, os);

  for(const Path& path : r) {
    svg.write(path, "stroke:rgb(0,0,0);stroke-width:1");

    for(const SegmentPtr seg : segments(path)) {
      if (segidx.find(*seg.get()) != segidx.end()) {
        svg.write(*seg.get(), "stroke:rgb(0,255,0);stroke-width:1");
        break;
      }
    }
  }
}

void plot_shared_points(const Route& r, const char* filename) {
  std::ofstream os(filename);
  SvgWriter svg(r, os);
  if(r.empty())
    return;

  std::vector<Point> sharedPoints;

  find_shared_points(r, sharedPoints);

  for(const Path& path : r) {
    svg.write(path, "stroke:rgb(0,0,0);stroke-width:10;");
  }

  for(const Point& p : sharedPoints) {
    svg.write(p, "stroke:rgb(255,0,0);stroke-width:10;fill:none");
  }

  svg.write(r.front().front(), "stroke:rgb(0,255,0);stroke-width:40;");
  svg.write(r.back().back(), "stroke:rgb(0,0,255);stroke-width:40;");
}

void plot_path_order(const Route& r, const char* filename) {
  std::ofstream os(filename);
  SvgWriter svg(r, os);
  std::vector<Point> sharedPoints;

  hsl_color hsl;
  rgb_color rgb;

  hsl.h = 0;
  hsl.s = 100;
  hsl.l = 50;

  float step = 360.0f / r.size();
  for(const Path& path : r) {
    hsl.h = (hsl.h+step);
    if (hsl.h >= 360) {
      hsl.h = 1;
    }
    rgb = hsl_to_rgb(hsl);
    string strokergb = (boost::format("stroke:rgb(%u,%u,%u)") % round(rgb.r) % round(rgb.g) % round(rgb.b)).str();
    svg.write(path, strokergb + ";stroke-width:10;fill:none;");

    svg.write(path.front(), "stroke:rgb(255,0,0);stroke-width:40;");
    svg.write(path.back(), "stroke:rgb(0,255,0);stroke-width:35;");
  }

  uint32_t count = 0;
  for(const Path& path : r) {
    svg.write(path.front(), "stroke:rgb(255,0,0);stroke-width:40;");
    svg.write(path.back(), "stroke:rgb(0,255,0);stroke-width:35;");
    svg.write((boost::format("%d") % count).str(), path.front(), "font-size=\"50\" fill=\"black\"");
    svg.write((boost::format("%d") % count).str(), path.back(), "font-size=\"50\" fill=\"black\"");

    ++count;
  }

  for(const Point& p : sharedPoints) {
    svg.write(p, "stroke:rgb(255,0,0);stroke-width:10;fill:none");
  }
}

void plot_segment_order(const Route& r, const char* filename) {
  std::ofstream os(filename);
  SvgWriter svg(r, os);

  hsl_color hsl;
  rgb_color rgb;

  hsl.h = 0;
  hsl.s = 100;
  hsl.l = 50;


  float step = 360.0f / segments(r).size();
  for(const SegmentPtr seg : segments(r)) {
    hsl.h = (hsl.h+step);
    if (hsl.h >= 360) {
      hsl.h = 1;
    }
    rgb = hsl_to_rgb(hsl);
    string strokergb = (boost::format("stroke:rgb(%u,%u,%u)") % round(rgb.r) % round(rgb.g) % round(rgb.b)).str();
    svg.write(*seg.get(), strokergb + ";stroke-width:10;");
 }

  uint32_t count = 0;
  Coord_t a,b;
  Point front, back, mark;

  for(const SegmentPtr seg : segments(r)) {
    front = seg.get()->front();
    back = seg.get()->back();
    a = std::abs(front.x - back.x);
    b = std::abs(front.y - back.y);

    mark.x = std::min(front.x, back.x) + a/2;
    mark.y = std::min(front.y, back.y) + b/2;

    svg.write(mark, "stroke:rgb(255,0,0);stroke-width:40;");
    svg.write((boost::format("%d") % count).str(), mark, "font-size=\"50\" fill=\"black\"");

    ++count;
  }
}

void plot_point_order(const Route& r, const char* filename) {
  std::ofstream os(filename);
  SvgWriter svg(r, os);

  hsl_color hsl;
  rgb_color rgb;

  hsl.h = 0;
  hsl.s = 100;
  hsl.l = 50;


  float step = 360.0f / segments(r).size();
  for(const SegmentPtr seg : segments(r)) {
    hsl.h = (hsl.h+step);
    if (hsl.h >= 360) {
      hsl.h = 1;
    }
    rgb = hsl_to_rgb(hsl);
    string strokergb = (boost::format("stroke:rgb(%u,%u,%u)") % round(rgb.r) % round(rgb.g) % round(rgb.b)).str();
    svg.write(*seg.get(), strokergb + ";stroke-width:10;");
  }

  uint32_t count = 0;
  Point last;
  for(const Path& path : r) {
    for(const Point& p : path) {
      if(p == last)
        return;
      svg.write(p, "fill:rgb(127,127,127);stroke-width:0;");
      svg.write((boost::format("%d") % count).str(), p, "font-size=\"50\" fill=\"black\"");
      ++count;
      last = p;
    }
  }
}

void plot_svg(const Route& r, const string& prefix) {
  plot_shared_points(r, (prefix + "_points_shared.svg").c_str());
  plot_shared_segments(r, (prefix + "_segments_shared.svg").c_str());
  plot_path_order(r, (prefix + "_path_order.svg").c_str());
  plot_segment_order(r, (prefix + "_segment_order.svg").c_str());
  plot_point_order(r, (prefix + "_point_order.svg").c_str());
}

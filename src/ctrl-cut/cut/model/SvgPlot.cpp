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

#include "SvgPlot.hpp"
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "svg/SvgWriter.hpp"
#include "cut/graph/SegmentGraph.hpp"
#include "cut/geom/Algorithms.hpp"

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

  BOOST_FOREACH(const Segment& seg, segments(r)) {
    g.add(seg);
  }

  BOOST_FOREACH(SegmentGraph::Vertex v, vertices(g)) {
    if (boost::degree(v, g) > 2) {
      sharedPoints.push_back(g[v]);
    }
  }
}

void plot_shared_segments(const Route& r, const char* filename) {
  std::set<Segment> segidx;
  uint32_t width = r.get(DocumentSettings::WIDTH).in(PX);
  uint32_t height = r.get(DocumentSettings::HEIGHT).in(PX);

  SvgWriter svg(width, height, filename);

  BOOST_FOREACH(const Path& path, r) {
    svg.write(path, "stroke:rgb(0,0,0);stroke-width:1");

    BOOST_FOREACH(const Segment& seg, segments(path)) {
      if (segidx.find(seg) != segidx.end()) {
        Path pseg = make_from(path);
        append(pseg, seg);
        svg.write(pseg, "stroke:rgb(0,255,0);stroke-width:1");
        break;
      }
    }
  }
}

void plot_shared_points(const Route& r, const char* filename) {
  uint32_t width = r.get(DocumentSettings::WIDTH).in(PX);
  uint32_t height = r.get(DocumentSettings::HEIGHT).in(PX);

  SvgWriter svg(width, height, filename);
  std::vector<Point> sharedPoints;

  find_shared_points(r, sharedPoints);

  BOOST_FOREACH(const Path& path, r) {
    svg.write(path, "stroke:rgb(0,0,0);stroke-width:10;");
  }

  BOOST_FOREACH(const Point& p, sharedPoints) {
    svg.write(p, "stroke:rgb(255,0,0);stroke-width:10;fill:none");
  }

  svg.write(r.front().front(), "stroke:rgb(0,255,0);stroke-width:40;fill:rgb(0,0,0)");
  svg.write(r.back().back(), "stroke:rgb(0,0,255);stroke-width:40;fill:rgb(0,0,0)");
}

void plot_path_order(const Route& r, const char* filename) {
  Coord_t width = r.get(DocumentSettings::WIDTH).in(PX);
  Coord_t height = r.get(DocumentSettings::HEIGHT).in(PX);

  SvgWriter svg(width, height, filename);
  std::vector<Point> sharedPoints;

  hsl_color hsl;
  rgb_color rgb;

  hsl.h = 0;
  hsl.s = 100;
  hsl.l = 50;

  float step = 360.0f / r.size();
  BOOST_FOREACH(const Path& path, r) {
    hsl.h = (hsl.h+step);
    if (hsl.h >= 360) {
      hsl.h = 1;
    }
    rgb = hsl_to_rgb(hsl);
    string strokergb = (boost::format("stroke:rgb(%u,%u,%u)") % round(rgb.r) % round(rgb.g) % round(rgb.b)).str();
    svg.write(path, strokergb + ";stroke-width:10;");

    svg.write(path.front(), "stroke:rgb(255,0,0);stroke-width:40;fill:rgb(0,0,0)");
    svg.write(path.back(), "stroke:rgb(0,255,0);stroke-width:35;fill:rgb(0,0,0)");
  }

  uint32_t count = 0;
  BOOST_FOREACH(const Path& path, r) {
    svg.write(path.front(), "stroke:rgb(255,0,0);stroke-width:40;fill:rgb(0,0,0)");
    svg.write(path.back(), "stroke:rgb(0,255,0);stroke-width:35;fill:rgb(0,0,0)");
    svg.text((boost::format("%d") % count).str(), path.front(), "font-size=\"50\" fill=\"black\"");
    svg.text((boost::format("%d") % count).str(), path.back(), "font-size=\"50\" fill=\"black\"");

    ++count;
  }

  BOOST_FOREACH(const Point& p, sharedPoints) {
    svg.write(p, "stroke:rgb(255,0,0);stroke-width:10;fill:none");
  }
}

void plot_segment_order(const Route& r, const char* filename) {
  Coord_t width = r.get(DocumentSettings::WIDTH).in(PX);
  Coord_t height = r.get(DocumentSettings::HEIGHT).in(PX);

  SvgWriter svg(width, height, filename);

  hsl_color hsl;
  rgb_color rgb;

  hsl.h = 0;
  hsl.s = 100;
  hsl.l = 50;


  float step = 360.0f / segments(r).size();
  BOOST_FOREACH(const Segment& seg, segments(r)) {
    hsl.h = (hsl.h+step);
    if (hsl.h >= 360) {
      hsl.h = 1;
    }
    rgb = hsl_to_rgb(hsl);
    string strokergb = (boost::format("stroke:rgb(%u,%u,%u)") % round(rgb.r) % round(rgb.g) % round(rgb.b)).str();
    Path p;
    append(p, seg);

    svg.write(p, strokergb + ";stroke-width:10;");
 }

  uint32_t count = 0;
  Coord_t a,b;
  Point front, back, mark;

  BOOST_FOREACH(const Segment& seg, segments(r)) {
    front = seg.front();
    back = seg.back();
    a = std::abs(front.x - back.x);
    b = std::abs(front.y - back.y);

    mark.x = std::min(front.x, back.x) + a/2;
    mark.y = std::min(front.y, back.y) + b/2;

    svg.write(mark, "stroke:rgb(255,0,0);stroke-width:40;fill:rgb(0,0,0)");
    svg.text((boost::format("%d") % count).str(), mark, "font-size=\"50\" fill=\"black\"");

    ++count;
  }
}

void plot_svg(const Route& r, const string& prefix) {
  plot_shared_points(r, (prefix + "_points_shared.svg").c_str());
  plot_shared_segments(r, (prefix + "_segments_shared.svg").c_str());
  plot_path_order(r, (prefix + "_path_order.svg").c_str());
  plot_segment_order(r, (prefix + "_segment_order.svg").c_str());
}

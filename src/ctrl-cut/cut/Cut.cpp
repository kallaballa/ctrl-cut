/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */

#include "Cut.hpp"

std::vector<CutPtr> load_cuts(DocumentSettings& ds, std::istream &input) {
  std::string line;
  char first;
  int power, x, y;
  float r, g, b;
  int lx = 0, ly = 0;
  int mx = 0, my = 0;
  LOG_INFO_STR("Load vector data");
  int segmentCnt = 0;

  //color to cut
  std::map<string,CutPtr> cuts;
  CutPtr current = CutPtr(new Cut(ds));
  cuts["0.0,0.0,0.0"] = current;

  while (std::getline(input, line)) {
    first = line[0];
    //std::cerr << line << std::endl;

    if (first == 'X') { // End of output
      break;
    }

    if (isalpha(first)) {
      switch (first) {
      case 'M': // move to
        if (sscanf(line.c_str() + 1, "%d,%d", &y, &x) == 2) {
          lx = x;
          ly = y;
          mx = x;
          my = y;
        }
        break;
      case 'C': // close
        if (lx != mx || ly != my) {
          segmentCnt++;
          add(*current.get(), Segment(Point(lx,ly),Point(mx,my)));
        }
        break;
      case 'R': // color change
        if (sscanf(line.c_str() + 1, "%f,%f,%f", &r, &g, &b) == 3) {
          string key = std::to_string(r) + std::to_string(g)+ std::to_string(b);
          auto it = cuts.find(key);
          if(it == cuts.end()) {
            current = CutPtr(new Cut(ds));
            cuts[key] = current;
          } else
            current = (*it).second;
        }
        break;
      case 'L': // line to
        if (sscanf(line.c_str() + 1, "%d,%d", &y, &x) == 2) {
          segmentCnt++;
          add(*current.get(), Segment(Point(lx,ly),Point(x,y)));
          lx = x;
          ly = y;
        }
        break;
      }
    }
  }

  LOG_DEBUG_MSG("loaded segments", segmentCnt);

  std::vector<CutPtr> result;
  for(auto pair : cuts) {
    if(!segments(pair.second).empty())
      result.push_back(pair.second);
  }
  return result;
}

/*!
 Loads vector data from EPS/Ghostscript output from the given file
 */
std::vector<CutPtr> load_cuts(DocumentSettings& ds, const std::string &filename) {
  std::ifstream infile(filename.c_str(), std::ios_base::in);
  return load_cuts(ds, infile);
}

Box find_bounding_box(Cut& cut) {
  Box b;
  Coord_t minx = std::numeric_limits<Coord_t>::max();
  Coord_t miny = std::numeric_limits<Coord_t>::max();
  Coord_t maxx = 0;
  Coord_t maxy = 0;
  Point translate = cut.get(CutSettings::CPOS);
  for(const Path& pt : cut) {
    for(const Point& p : pt) {
      minx = std::min(p.x + translate.x, minx);
      miny = std::min(p.y + translate.y, miny);
      maxx = std::max(p.x + translate.x, maxx);
      maxy = std::max(p.y + translate.y, maxy);
    }
  }

  return Box(minx, miny, maxx, maxy);
}

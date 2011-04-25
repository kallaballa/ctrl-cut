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
#include "Cut.h"
#include "Vertex.h"
#include "Edge.h"
#include "Polyline.h"

#include <fstream>
#include <algorithm>
#include <boost/format.hpp>

using boost::format;

void Cut::add(Polyline* pl)
{
  this->polylines.push_back(pl);
}

void Cut::remove(Polyline* pl)
{
  Cut::iterator it = find(this->begin(), this->end(), pl);
  if (it != this->end()) {
    this->polylines.erase(it);
  }
}

bool Cut::contains(Polyline* pl) const {
  Cut::const_iterator it = find(this->begin(), this->end(), pl);
  return it != this->end();
}

/*!
  Loads vector data from EPS/Ghostscript output
*/
Cut *Cut::load(std::istream &input)
{
  Cut *cut = new Cut();
  std::string line;
  char first;
  int power, x, y;
  int lx = 0, ly = 0;
  int mx = 0, my = 0;
  Mesh &mesh = cut->mesh;

  while (std::getline(input, line)) {
    first = line[0];

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
          mesh.create(lx, ly, mx, my);
        }
        break;
      case 'P': // power
        if (sscanf(line.c_str() + 1, "%d", &x) == 1) {
          // FIXME: While testing, ignore the strange color-intensity-is-power convension
          //          power = x;
          power = -1;
        }
        break;
      case 'L': // line to
        if (sscanf(line.c_str() + 1, "%d,%d", &y, &x) == 2) {
          mesh.create(lx, ly, x, y);
          lx = x;
          ly = y;
        }
        break;
      }
    }
  }

  if (mesh.empty()) {
    delete cut;
    cut = NULL;
  }
  return cut;
}

/*!
  Loads vector data from EPS/Ghostscript output from the given file
*/
Cut *Cut::load(const std::string &filename)
{
  std::ifstream infile(filename.c_str(), std::ios_base::in);
  return Cut::load(infile);
}

/*!
  Writes this cut to XML with the given filename
 */
void Cut::writeXml(const std::string &filename) const
{
  std::ofstream os(filename.c_str(), std::ios_base::out);
  os << *this;
  os.close();
}

void Cut::writeSvg(const std::string &filename) const
{
  std::ofstream os(filename.c_str(), std::ios_base::out);
  os << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"no\"?><svg xmlns=\"http://www.w3.org/2000/svg\" ";

  BBox totalbb;
  for (Cut::const_iterator it = this->begin(); it != this->end(); it++) {
    Polyline *p = *it;
    totalbb.extendBy(p->getBoundingBox());
  }
  int w, h;
  totalbb.getSize(w, h);
  os << "height=\"" << h << "\" width=\"" << w << "\"";
  os << " version=\"1.1\" x=\"test-data/vector/x-line-2.raw\" preserveAspectRatio=\"xMidYMid meet\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"";
  os << totalbb.ul[0] << " " << totalbb.ul[1] << " " << w << " " << h;
  os << "\"  >\n";

  for (Cut::const_iterator it = this->begin(); it != this->end(); it++) {
    Polyline *p = *it;
    os << "<polyline style=\"stroke:black; stroke-width:0.001mm; fill:none; \" points=\"";
    Edge *e = *(p->begin());
    os << (*(e->start()))[0] << "," << (*(e->start()))[1] << " ";
    for (Polyline::iterator it = p->begin(); it != p->end(); it++) {
      e = *it;
      os << (*(e->end()))[0] << "," << (*(e->end()))[1] << " ";
    }
    os << "\"  />\n";
  }
  os << "</svg>\n";
  os.close();
}

std::ostream &operator<<(std::ostream &os, const Cut &cut) {
  os << "<cut clipped=\"" << cut.wasClipped() << "\">" << std::endl;
  os << "<polylines cnt=\"" << cut.size() << "\" >" << std::endl;
  for (Cut::const_iterator it = cut.begin(); it != cut.end(); it++) {
    os << **it;
  }
  os << "</polylines>" << std::endl;

  os << cut.getMesh() << std::endl;

  os << "</cut>" << std::endl;

  return os;
}

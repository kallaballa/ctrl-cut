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

using boost::format;

void Cut::add(Polyline* pl) {
  this->std::vector<Polyline *>::push_back(pl);
}

void Cut::remove(Polyline* pl) {
  Cut::iterator it = this->find(pl);
  if (it != (Cut::iterator) NULL)
    this->std::vector<Polyline *>::erase(it);
}

bool Cut::contains(Polyline* pl) {
  Cut::iterator it = this->find(pl);
  return it != (Cut::iterator) NULL && it != this->end();
}

Cut *Cut::load(istream &input)
{
  Cut *cut = new Cut();
  string line;
  char first;
  int power, x, y;
  int lx, ly;
  int mx, my;
  Vertex *start;
  Vertex *end;
  Mesh& mesh = cut->getMesh();

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
          mesh.createEdge(new Vertex(lx, ly), new Vertex(mx, my), power);
        }
        break;
      case 'P': // power
        if (sscanf(line.c_str() + 1, "%d", &x) == 1) {
          // FIXME: While testing, ignore the strange color-intensity-is-power convension
          //          power = x;
          power = VECTOR_POWER_DEFAULT;
        }
        break;
      case 'L': // line to
        if (sscanf(line.c_str() + 1, "%d,%d", &y, &x) == 2) {
          start = new Vertex(lx, ly);
          end = new Vertex(x, y);
          mesh.createEdge(start, end, power);
          lx = x;
          ly = y;
        }
        break;
      }
    }
  }

  return cut;
}

Cut *Cut::load(const string &filename)
{
  ifstream infile(filename.c_str(), ios_base::in);
  return Cut::load(infile);
}

void Cut::xml(std::string s) {
  ofstream os(s.c_str(), ios_base::out);
  Cut c = *((Cut*)this);
  os << c;
  os.close();
}

ostream& operator<< (ostream &os, Cut &cut) {
  os << "<cut clipped=\"" << cut.wasClipped() << "\">" << std::endl;
  os << "<polylines cnt=\"" << cut.size() << "\" >" << std::endl;
  for(Cut::iterator it = cut.begin(); it != cut.end(); it++) {
    os << *((Polyline*)*it);
  }
  os << "</polylines>" << std::endl;

  os << cut.getMesh() << std::endl;

  os << "</cut>" << std::endl;

  return os;
}

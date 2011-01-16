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

/*!
  Adds the given edge to the vector pass.
  power is the laser power in percent.
  
  FIXME: Speed and frequency should be specified as well
*/
void Cut::createEdge(Vertex *start, Vertex *end, int power)
{
	//filter zero length edges
	if(start->getKey() == end->getKey())
		return;

  // FIXME: Clip against page size
  if (start->getX() < 0 || start->getY() < 0 ||
      end->getX() < 0 || end->getY() < 0) {

    if (start->getX() < 0) start->setX(0);
    if (start->getY() < 0) start->setY(0);
    if (end->getX() < 0) end->setX(0);
    if (end->getY() < 0) end->setY(0);

    // FIXME: The Windows driver subtracts 1 point from the X
    // coordinate of the end of any line segment which is
    // clipped. Strange, but let's follow suit for now.
    end->setX(end->getX()-1);

    this->clipped = true;
  }

  Edge *ls = new Edge(start, end, power);

  start = mapVertex(start);
  ls->setStart(start);
  start->attach(ls);

  end = mapVertex(end);
  ls->setEnd(end);
  end->attach(ls);

  freeEdges.push_back(ls);
}

Vertex *Cut::mapVertex(Vertex *p)
{
  MapVertex::iterator it = vertices.find(p->getKey());
  
  if (it != vertices.end()) {
    return (Vertex *)it->second;
  }
  
  vertices.insert(pair<string, Vertex*> (p->getKey(), p));
  return p;
}

void Cut::removeEdge(Edge* e, bool detach) {
  if(detach)
    e->detach();
  freeEdges.remove(e);
}

LstEdge::iterator Cut::removeEdge(LstEdge::iterator it_e, bool detach) {
  Edge *e = *it_e;
  if(detach)
    e->detach();

  return freeEdges.erase(it_e);
}

Cut *Cut::load(const string &filename) {
  Cut *cut = new Cut();
  string line;
  ifstream infile(filename.c_str(), ios_base::in);
  char first;
  int power, x, y;
  int lx, ly;
  int mx, my;
  Vertex *start;
  Vertex *end;

  while (std::getline(infile, line)) {
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
          cut->createEdge(new Vertex(lx, ly), new Vertex(mx, my), power);
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
          cut->createEdge(start, end, power);
          lx = x;
          ly = y;
        }
        break;
      }
    }
  }
  infile.close();

  return cut;
}

/*
  void Cut::print(ostream &stream)
  {
  Edge *ls;
  for (EdgeList::iterator it_s = this->freeEdges.begin();  it_s != this->freeEdges.end(); it_s++) {
  ls = *it_s;

  stream << ls->getStart()->getX() << "," << ls->getStart()->getY() << "  -  "
  << ls->getEnd()->getX() << "," << ls->getEnd()->getY() << "\n";
  }


  EdgeList skin_segm;
  EdgeList::reverse_iterator it_i;

  if (skins.size() > 0) {
  stream << "Skins: " << skins.size() << "\n";
    
  for (int i = skins.size() - 1; i >= 0; i--) {
  skin_segm = skins.at(i)->getLineSegments();
  printf("s: %d\n", skin_segm.size());
      
  for (it_i = skin_segm.rbegin(); it_i != skin_segm.rend(); it_i++) {
  ls = *it_i;
  if (it_i != skin_segm.rbegin())
  ls->invertDirection();
  if (ls != NULL) {
  //print_line(ls);
  stream << "P";
  stream << ls->getPower();
  stream << "\n";
  stream << "M";
  stream << ls->getStart()->getX();
  stream << ",";
  stream << ls->getStart()->getY();
  stream << "\n";
  stream << "L";
  stream << ls->getStart()->getX();
  stream << ",";
  stream << ls->getStart()->getY();
  stream << "\n";
  stream << "L";
  stream << ls->getEnd()->getX();
  stream << ",";
  stream << ls->getEnd()->getY();
  stream << "\n";
  }
  }
  }
  }
  stream << "X\n";
  }

*/

void Cut::xml(std::string s) {
  ofstream os(s.c_str(), ios_base::out);
  Cut c = *((Cut*)this);
  os << c;
  os.close();
}

ostream& operator<< (ostream &os, Cut &cut) {
  os << "<cut clipped=\"" << cut.wasClipped() << "\">" << std::endl;

  os << "<edges cnt=\"" << cut.freeEdges.size() << "\" >" << std::endl;
  for(LstEdge::iterator it = cut.freeEdges.begin(); it != cut.freeEdges.end(); it++) {
    os << *((Edge*)*it);
  }
  os << "</edges>" << std::endl;
  os << "<polylines cnt=\"" << cut.polylines.size() << "\" >" << std::endl;
  for(VecPolyline::iterator it = cut.polylines.begin(); it != cut.polylines.end(); it++) {
    os << *((Polyline*)*it);
  }
  os << "</polylines>" << std::endl;
  os << "<vertices cnt=\"" << cut.vertices.size() << "\" >" << std::endl;
  for(MapVertex::iterator it = cut.vertices.begin(); it != cut.vertices.end(); it++) {
    Vertex* vec = (Vertex*) (*it).second;
    os << *vec;
  }
  os << "</vertices>" << std::endl;
  os << "</cut>" << std::endl;
  return os;
}

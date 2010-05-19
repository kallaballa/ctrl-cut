#include "VectorPass.h"

#include "pjl.h"
#include "LaserJob.h"

#include <fstream>
#include <iostream>
#include "boost/format.hpp"

using boost::format;

/*!
  Loads a vector file and constructs a VectorPass.
*/
VectorPass *VectorPass::createFromFile(const string &filename)
{
  VectorPass *vpass = new VectorPass();
  string line;
  ifstream infile(filename.c_str(), ios_base::in);
  char first;
  int power, x, y;
  int lx, ly;
  int mx, my;
  Joint *start;
  Joint *end;

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
          vpass->addLine(new Joint(lx, ly), new Joint(mx, my), power);
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
          start = new Joint(lx, ly);
          end = new Joint(x, y);
          vpass->addLine(start, end, power);
          lx = x;
          ly = y;
        }
        break;
      }
    }
  }
  infile.close();

  return vpass;
}

/*!
  Adds the given line segment to the vector pass.
  power is the laser power in percent.
  
  FIXME: Speed and frequency should be specified as well
*/
void VectorPass::addLine(Joint *start, Joint *end, int power)
{
  LineSegment *ls = new LineSegment(start, end, power);

  start = addJoint(start);
  ls->setStart(start);
  start->addConnector(ls);

  end = addJoint(end);
  ls->setEnd(end);
  end->addConnector(ls);

  lines.push_back(ls);
}

Joint *VectorPass::addJoint(Joint *p)
{
  JointMap::iterator it = joints.find(p->getKey());
  
  if (it != joints.end()) {
    return (Joint *)it->second;
  }
  
  joints.insert(pair<string, Joint*> (p->getKey(), p));
  return p;
}

LineSegmentList::iterator VectorPass::eraseLine(LineSegmentList::iterator it_ls) {
  LineSegment *ls = *it_ls;
  ls->getStart()->removeConnector(ls);
  ls->getEnd()->removeConnector(ls);

  return lines.erase(it_ls);
}

void VectorPass::addOnionSkin(OnionSkin* skin) {
  skins.push_back(skin);
}

void VectorPass::print(ostream &stream)
{
  LineSegment *ls;
  for (LineSegmentList::iterator it_s = this->lines.begin();  it_s != this->lines.end(); it_s++) {
    ls = *it_s;

    stream << ls->getStart()->getX() << "," << ls->getStart()->getY() << "  -  "
           << ls->getEnd()->getX() << "," << ls->getEnd()->getY() << "\n";
  }


  LineSegmentList skin_segm;
  LineSegmentList::reverse_iterator it_i;

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

/**
 *
 */
void VectorPass::serializeTo(ostream &out)
{
  bool first = true;
  bool writingPolyline = false;

  out << V_INIT << SEP;
  out << format(V_POWER) % this->lconf->vector_power << SEP;
  out << format(V_SPEED) % this->lconf->vector_speed << SEP;
  out << format(V_FREQUENCY) % this->lconf->vector_freq << SEP;

  LineSegment* ls;
  int beginX = -1, beginY = -1;
  int lastX = -1, lastY = -1;
  int lastPower = this->lconf->vector_power;
  for (LineSegmentList::iterator it_s = this->lines.begin(); 
       it_s != this->lines.end(); it_s++) {
    ls = *it_s;

    int power = ls->getPower();
    if (power != lastPower) {
      if (writingPolyline) {
        out << SEP;
        writingPolyline = false;
      }
      int epower = (power * this->lconf->vector_power + 50) / 100;
      if (this->lconf->vector_speed && this->lconf->vector_speed < 100) {
        int espeed = this->lconf->vector_speed;
        int efreq = this->lconf->vector_freq;
        if (epower && power < 100) {
          int r;
          int q;
          r = 10000 / power; // power, up to set power level (i.e. x=100)
          q = 10000 / espeed;
          if (q < r) r = q;
          q = 500000 / efreq;
          if (q < r) r = q;
          epower = (50 + epower * r) / 100;
          espeed = (50 + espeed * r) / 100;
          efreq = (50 + espeed * r) / 100;
        }

        out << SEP << format(V_SPEED) % espeed;
        out << SEP << format(V_FREQUENCY) % efreq << SEP;
      }
      out << format(V_POWER) % epower << SEP;
    }


    int startX = this->lconf->basex + ls->getStart()->getX() + HPGLX;
    int startY = this->lconf->basey + ls->getStart()->getY() + HPGLY;
    int endX = this->lconf->basex + ls->getEnd()->getX() + HPGLX;
    int endY = this->lconf->basey + ls->getEnd()->getY() + HPGLY;
    
    if (beginX < 0) {
      beginX = startX;
      beginY = startY;
    }

    // After a power change, always issue a PU, even if the current
    // coordinate doesn't change.
    if (lastX != startX || lastY != startY || lastPower != power) {
      if (first) {
        out << HPGL_PEN_UP_INIT;
        first = false;
      } else {
        out << HPGL_PEN_UP;
      }
      out << format("%d,%d") % startX % startY << SEP;
      out << HPGL_PEN_DOWN << format("%d,%d") % endX % endY;
      writingPolyline = true;
    }
    // ..else continue on polyline
    else {
      out << format(",%d,%d") % endX % endY;
    }

    lastX = endX;
    lastY = endY;
    lastPower = power;
  }

  // FIXME: This is a temporary hack to emulate the Epilog Windows driver,
  // which appears to repeat the first vertex in a closed polyline twice at the end.
  if (beginX >= 0) {
    out << format(",%d,%d") % beginX % beginY;
  }


  out << SEP << HPGL_END << PCL_SECTION_END << HPGL_PEN_UP;
}


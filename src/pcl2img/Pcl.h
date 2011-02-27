/*
 * EpilogCUPS - A laser cutter CUPS driver
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

#ifndef PCLFILE_H_
#define PCLFILE_H_

#include "stdint.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include <cctype>
#include <boost/format.hpp>
#include <map>
#include <list>
#include "Signatures.h"
#include "CImg.h"

using std::cerr;
using std::endl;
using std::string;
using std::fstream;
using std::ostream;
using std::stringstream;
using std::ios;
using std::map;
using std::list;
using std::numeric_limits;
using boost::format;
using namespace cimg_library;


#define PCL_COORD_MAX numeric_limits<uint32_t>::max()
#define PCL_DIM_MAX numeric_limits<uint32_t>::max()

typedef uint32_t coord;
typedef uint32_t dim;

char magic[] = { 0x1b, 0x25, 0x2d, 0x31, 0x32, 0x33, 0x34,
        0x35, 0x58, 0x40, 0x50, 0x4a, 0x4c, 0x20, 0x4a, 0x4f, 0x42, 0x20, 0x4e,
        0x41, 0x4d, 0x45, 0x3d };

class Point {
public:
  coord x;
  coord y;
  Point(coord x, coord y): x(x), y(y) {}
  friend ostream& operator <<(ostream &os, Point &p) {
    os << "(" << p.x << "," << p.y << ")";
    return os;
  }
};

class BoundingBox {
public:
  Point ul;
  Point lr;

  BoundingBox(): ul(PCL_COORD_MAX, PCL_COORD_MAX), lr(0,0){}

  void reset(){
    this->ul.x=PCL_COORD_MAX;
    this->ul.y=PCL_COORD_MAX;
    this->lr.x=0;
    this->lr.y=0;
  }

  void update(Point p) {
    update(p.x, p.y);
  }

  void update(coord x, coord y=PCL_COORD_MAX) {
      this->ul.x = min(this->ul.x, x);
      this->lr.x = max(this->lr.x, x);

      if(y != PCL_COORD_MAX) {
        this->ul.y = min(this->ul.y, y);
        this->lr.y = max(this->lr.y, y);
      }
  }

  coord min(coord c1, coord c2) {
    return (c1<=c2?c1:c2);
  }

  coord max(coord c1, coord c2) {
    return (c1>=c2?c1:c2);
  }

  friend ostream& operator <<(ostream &os, BoundingBox &bbox) {
    os << "(" << bbox.ul << "," << bbox.lr << ")";
    return os;
  }
};

class PclInstr {
public:
  PclInstr(off64_t file_off) : type(255), prefix(255), suffix(255), keysep('\0'), value(0), pos(0), limit(0), file_off(file_off), hasValue(false), hasData(false) {}
  char type;
  char prefix;
  char suffix;
  char keysep; // separate the instr signature from value/data so it may be interpreted as a string and used as key
  int32_t value;
  uint8_t * data;
  uint8_t pos;
  uint8_t limit;
  off64_t file_off;

  bool hasValue;
  bool hasData;

  bool hasNext() {
    return this->pos < this->limit;
  }

  uint8_t next(){
      return (uint8_t) *(this->data+this->pos++);
  }

  bool matches(const string& signature, bool report=false) {
    bool m = memcmp(this, signature.c_str(), 3) == 0;
    if(!m && report)
      cerr << "expected: " << signature << " found: " << (char*)this << endl;

    return m;
  }

  static const string pretty(char c) {
    if(!isgraph(c))
      return (format("(0x%02X)") % c).str();
    else
      return (format("%c") % c).str();
  }

  friend ostream& operator <<(ostream &os, PclInstr &instr) {
    format fmtInstr("(%08X) %s%s%s = %d");
    fmtInstr % instr.file_off
        % PclInstr::pretty(instr.type)
        % PclInstr::pretty(instr.prefix)
        % PclInstr::pretty(instr.suffix);

    if(instr.hasValue)
      fmtInstr % instr.value;
    else
      fmtInstr % "NULL";

    return os << fmtInstr;
  }
};

class Trace {
private:
  const uint8_t backlogSize;
  list<PclInstr*> backlog;
  static Trace* instance;

  Trace(): backlogSize(10) {

  }
public:
  static Trace* singleton();

  void log(PclInstr* instr) {
    if(backlog.size() >= backlogSize)
      backlog.erase(backlog.begin());

    backlog.push_back(instr);
  }

  list<PclInstr*>::iterator backlogIterator() {
    return backlog.begin();
  }

  list<PclInstr*>::iterator backlogEnd() {
    return backlog.end();
  }

  void printBacklog(ostream &os, string msg) {
    os << "=== trace: " << msg << ": " << endl;
    if(backlog.empty()){
      os << "(backlog N/A)" << endl;
    }else{
      for(list<PclInstr*>::iterator it = backlogIterator(); it != backlogEnd(); it++)
        os << *(*it) << endl;
    }
    os << endl;
  }
};

Trace* Trace::instance = NULL;
Trace* Trace::singleton(){
  if(instance == NULL)
    instance = new Trace();

  return instance;
}

class PclPlot {
private:
  fstream inputfile;
  const char* title;
  const uint16_t bufSize;
  char* buffer;
  off64_t eof;

  bool valid;
  std::map<string, PclInstr*> settings;

  bool readPattern(const char * pattern, const int off, const int len) {
    if (this->inputfile.read(buffer, len)) {
      return memcmp((pattern + off), buffer, len) == 0;
    } else
      return false;
  }

  off64_t fillBuffer() {
    off64_t off = this->inputfile.tellg();

    for (int i = 0; i < (bufSize - 1) && this->inputfile.good(); i++) {
      buffer[i] = this->inputfile.get();
      if (buffer[i] == PCL_END_OF_INSTRUCTION) {
        buffer[i] = '\0';
        return off;
      }
    }

    return eof;
  }

  void readHeader() {
    if (!readPattern(magic,0, 23))
      invalidate("read magic");

    if (fillBuffer() == eof)
      invalidate("read title");
    else
      this->title = this->buffer;

    if (fillBuffer() == eof)
      invalidate("skip pcl intro");
  }

  void readSettings() {
    PclInstr* instr;
    while ((instr = readInstr()) && !instr->matches(PCL_END_OF_SETTINGS)) {
      settings[(char*) instr] = instr;
    }
    printSettings(cerr);
  }
public:
  PclInstr* currentInstr;

  PclPlot(char* filename) : inputfile(filename, ios::in | ios::binary), bufSize(1024), buffer(new char[1024]), eof(numeric_limits<off64_t>::max()), valid(true) {
    readHeader();
    readSettings();
  };
  virtual ~PclPlot(){};

  void printSettings(ostream& os) {
    std::map<string, PclInstr*>::iterator it;
    os << "=== settings: " << endl;
    for (it = settings.begin(); it != settings.end(); it++) {
      os << *((*it).second) << endl;
    }
  }

  bool require(string signature) {
    if(isSet(signature))
      return true;
    else
      invalidate("missing setting: " + signature);

    return false;
  }

  bool isSet(string signature) {
    return this->settings.find(signature) != settings.end();
  }

  uint32_t setting(string signature) {
    return this->settings[signature]->value;
  }

  void invalidate(string msg) {
    this->valid = false;
    Trace::singleton()->printBacklog(cerr, msg);
  }

  bool good() {
    return this->valid && this->inputfile.good();
  }

  PclInstr* readInstr(const char * expected=NULL) {
    if(!this->inputfile.good()) {
      invalidate("broken pipe");
      return NULL;
    }
    off64_t fileoff = fillBuffer();

    if(fileoff == eof) {
      invalidate("end of file");
      return NULL;
    }

    char* token = this->buffer;
    if (token) {
      PclInstr* instr = new PclInstr(fileoff);
      this->currentInstr = instr;
      char * endptr;
      char * cursor = token;
      instr->type = *cursor;
      instr->prefix = *(++cursor);
      char* cval = ++cursor;
      instr->value = strtol(cval, &endptr, 10);

      if (endptr <= cursor) {
        instr->suffix = *cval;
      }
      else {
        instr->suffix = *endptr;
        instr->hasValue = true;
      }

      Trace::singleton()->log(instr);
      if(expected && !instr->matches(expected, true)) {
        return NULL;
      } else {
        if (instr->matches(PCL_RLE_DATA)) {
          //copy data part of rle instruction
          instr->data = new uint8_t[instr->value];
          instr->limit = instr->value;
          memcpy(instr->data, (++endptr), instr->value);
          instr->hasData = true;
        }

        return instr;
      }
    }
    //FIXME invalidate?
    if(expected) {
      cerr << "failed to read: " << expected << std::endl;
    }
    return NULL;
  }
};

class PclPlotter {
private:
  CImg<uint8_t>* img;
  Point penPos;
  Point relPos;
  Point flipAt;
  bool flip;
  bool down;

public:
  uint8_t intensity;
  Point origin;

  PclPlotter(dim width, dim height): img(new CImg<uint8_t>(width, height, 1, 1, 255)), penPos(0,0), relPos(0,0), flipAt(0,0), flip(false), down(false), intensity(0), origin(0,0) {};

  void doFlip(Point& at) {
    this->flip = !this->flip;

    this->flipAt.x += at.x;
    this->flipAt.y = at.y;

    if (this->flip)
      this->penPos.y -= (at.y - this->flipAt.y);
    else
      this->penPos.y += (at.y - this->flipAt.y);
  }

  void penUp() {
    down = false;
  }

  void penDown() {
    down = true;
  }

  void move(Point& to) {
    this->move(to.x, to.y);
  }

  void move(coord x, coord y) {
    if(relPos.x == x && relPos.y == y)
      return;

    if(down) {
      uint8_t color [1] = { intensity };
      img->draw_line(relPos.x, relPos.y, x, y, color);
    }

    relPos.x = x;
    relPos.y = y;
  }

  CImg<uint8_t>* getCanvas() {
    return img;
  }
};

#endif /* PCLFILE_H_ */

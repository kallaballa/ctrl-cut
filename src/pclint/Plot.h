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

#ifndef PLOTTER_H_
#define PLOTTER_H_

#include <map>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include <cctype>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <limits>
#include "CImg.h"
#include "2D.h"

using std::ios;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::fstream;
using std::numeric_limits;

using namespace cimg_library;

class PclPlotter {
private:
  BoundingBox *crop;

  Point relPos;

  bool flip;
  bool down;
  coord flipX;
  CImg<uint8_t> *img;

public:
  uint8_t intensity;
  Point penPos;

  PclPlotter(Point& startPos, dim width, dim height, BoundingBox* crop = NULL):
    crop(crop), flip(false), down(false), intensity(0)
  {
    if(crop != NULL) {
      width = crop->min(width, crop->lr.x - crop->ul.x);
      height = crop->min(height, crop->lr.y -  crop->ul.y);
    }

    this->img = new CImg<uint8_t>(width, height, 1, 1, 255);
    this->penPos = startPos;
    this->relPos = startPos;
  };

  void doFlip(Point& at) {
    this->flip = !this->flip;
    /*if (!this->flip)
      this->penPos.x -= flipX;
    else {
      this->penPos.x += (this->relPos.x - at.x);
      flipX = (this->relPos.x - at.x);
    }*/
  }

  void penUp() {
    down = false;
  }

  void penDown() {
    down = true;
  }

  void move(coord x, coord y) {
    Point m(x,y);
    move(m);
  }

  void move(Point& to) {
    coord crop_offX = 0;
    coord crop_offY = 0;

    if(this->crop) {
      to = this->crop->shape(to);
      crop_offX = crop->ul.x;
      crop_offY = crop->ul.y;
    }

    Point newPenPos;

    newPenPos = to;

   /* if (this->flip)
      newPenPos.y = (this->penPos.y) + (this->relPos.y - to.y);
    else*/
      newPenPos.y = (this->penPos.y) - (this->relPos.y - to.y);

    newPenPos.x = (this->penPos.x) - (this->relPos.x - to.x);

    if(relPos != to) {
      if(down) {
        uint8_t color [1] = { intensity };
        img->draw_line(penPos.x - crop_offX, penPos.y - crop_offY, newPenPos.x - crop_offX, newPenPos.y - crop_offY, color);
        cerr << "\t\tmove: " << penPos << endl;
      }
      this->relPos = to;
      this->penPos = newPenPos;
      Trace::singleton()->logPlotterStat(penPos, relPos);
    }
  }

  CImg<uint8_t>* getCanvas() {
    return img;
  }
};

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
    return this->inputfile.read(buffer, len)
        ? memcmp((pattern + off), buffer, len) == 0
            : false;
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

  PclPlot(const char* filename):
    inputfile(filename, ios::in | ios::binary),
    bufSize(1024), buffer(new char[1024]),
    eof(numeric_limits<off64_t>::max()), valid(true) {
    readHeader();
    readSettings();
  };
  virtual ~PclPlot(){};

  void printSettings(ostream& os) {
    std::map<string, PclInstr*>::iterator it;
    os << "=== settings: " << endl;
    for (it = settings.begin(); it != settings.end(); it++) {
      os << "\t" << *((*it).second) << endl;
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

      Trace::singleton()->logInstr(instr);

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

#endif /* PLOTTER_H_ */
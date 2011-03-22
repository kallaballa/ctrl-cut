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
#include <boost/format.hpp>
#include "CImg.h"
#include "2D.h"
#include "PclIntConfig.h"

using std::ios;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::fstream;
using std::numeric_limits;
using boost::format;

using namespace cimg_library;

class PclPlotter {
private:
  BoundingBox *bbox;
  BoundingBox *clip;
  bool down;
  CImg<uint8_t> *img;
  uint8_t intensity[1];

public:
  Point penPos;

  PclPlotter(dim width, dim height, BoundingBox* clip = NULL) :
    bbox(new BoundingBox()), clip(clip), down(false), penPos(0, 0) {
    if (clip != NULL) {
      width = clip->min(width, clip->lr.x - clip->ul.x);
      height = clip->min(height, clip->lr.y - clip->ul.y);
    }

    this->img = new CImg<uint8_t> (width, height, 1, 1, 255);
  }
  ;

  PclPlotter(BoundingBox* clip = NULL) :
    bbox(new BoundingBox()), clip(clip), down(false), penPos(0, 0) {
    this->img = NULL;
  }
  ;

  void penUp() {
    down = false;
  }

  void penDown() {
    down = true;
  }

  void move(coord x, coord y) {
    Point m(x, y);
    move(m);
  }

  void setIntensity(uint8_t intensity) {
    this->intensity[0] = intensity;
  }

  uint8_t getIntensity() {
    return this->intensity[0];
  }

  virtual void draw(const Point& from, const Point& to) {
    if(from.y != to.y) {
      cerr << "non horizontal draw operation?" << endl;
      return;
    }

    if(from == to) {
      cerr << "zero length drawing operation?" << endl;
      return;
    }

    Point drawFrom;
    Point drawTo;

    //assume all drawing operations are horizontal and always work from left to right
    if(from.x < to.x) {
      drawFrom = from;
      drawTo = to;
    } else {
      drawFrom = to;
      drawTo = from;
    }

    coord clip_offX = 0;
    coord clip_offY = 0;

    //apply clipping and update bounding box
    if (this->clip) {
      drawTo = this->clip->shape(drawTo);
      clip_offX = clip->ul.x;
      clip_offY = clip->ul.y;
    }

    // x coordinates point to the left of a pixel. therefore don't draw the last coordinate
    // This is done before the bbox calculation to avoid an off-by-one error as the bbox
    // is specified in pixels, inclusive the end pixels.
    drawTo.x--;

    this->bbox->update(drawFrom);
    this->bbox->update(drawTo);

    drawFrom.x -= clip_offX;
    drawFrom.y -= clip_offY;
    drawTo.x -= clip_offX;
    drawTo.y -= clip_offY;

    cerr << "\t\t" << drawFrom << " - " << drawTo << " i = " << (unsigned int)this->intensity[0] << endl;

    img->draw_line(drawFrom.x, drawFrom.y, drawTo.x, drawTo.y, this->intensity);
  }

  void move(Point& to1) {
    Point to = to1;
    if (penPos != to) {
      if (down) {
        draw(penPos, to);
      }
      this->penPos = to;
      Trace::singleton()->logPlotterStat(penPos);
    }
  }

  virtual BoundingBox* getBoundingBox() {
    return bbox;
  }

  virtual CImg<uint8_t>* getCanvas() {
    if (PclIntConfig::singleton()->autocrop) {
      return &(img->crop(this->bbox->ul.x, this->bbox->ul.y, this->bbox->lr.x, this->bbox->lr.y, false));
    }
    else {
      return img;
    }
  }
};

class PclPlot {
private:
  fstream inputfile;
  string title;
  const uint16_t bufSize;
  char* buffer;
  off64_t eof;
  bool valid;

  std::map<string, PclInstr*> settings;

  bool readPattern(const char * pattern, const int off, const int len) {
    return this->inputfile.read(buffer, len) ? memcmp((pattern + off), buffer,
        len) == 0 : false;
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
    if (!readPattern(MAGIC, 0, MAGIC_SIZE))
      invalidate("read magic");

    if (fillBuffer() == eof)
      invalidate("read title");
    else
      this->title = string(this->buffer);

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

  PclPlot(const char* filename) :
    inputfile(filename, ios::in | ios::binary), bufSize(1024), buffer(
        new char[1024]), eof(numeric_limits<off64_t>::max()), valid(true) {
    readHeader();
    readSettings();
  }

  virtual ~PclPlot() {
  }

  void printSettings(ostream& os) {
    std::map<string, PclInstr*>::iterator it;
    os << "=== settings: " << endl;
    for (it = settings.begin(); it != settings.end(); it++) {
      os << "\t" << *((*it).second) << endl;
    }
  }

  bool require(string signature) {
    if (isSet(signature))
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

  int32_t* readValue() {
    stringstream ss;
    char digit;
    for (int i = 0; this->inputfile.good() && (isdigit(digit
        = this->inputfile.get()) || digit == '-'); i++) {
      ss << digit;
    }

    if (!this->inputfile.good()) {
      invalidate("broken pipe");
      return NULL;
    }

    this->inputfile.unget();

    string strval = ss.str();

    if (strval.size() > 0) {
      int32_t* val = new int32_t();
      *val = static_cast<int32_t> (strtoll(strval.c_str(), NULL, 10));
      return val;
    }
    return NULL;
  }

  PclInstr* readInstr(const char * expected = NULL) {
    if (!this->inputfile.good()) {
      invalidate("broken pipe");
      return NULL;
    }

    off64_t fileoff = this->inputfile.tellg();

    PclInstr* instr = new PclInstr(fileoff);
    this->currentInstr = instr;
    instr->type = this->inputfile.get();
    instr->prefix = this->inputfile.get();
    int32_t * pval = readValue();
    instr->suffix = this->inputfile.get();

    if (pval != NULL) {
      instr->value=*pval;
      instr->hasValue = true;
    }

    Trace::singleton()->logInstr(instr);

    // copy data part of rle instructions
    if (instr->matches(PCL_RLE_DATA)) {
      instr->data = new uint8_t[instr->value];
      instr->limit = instr->value;
      instr->hasData = true;

      for (int i = 0; i < instr->limit && this->inputfile.good(); i++)
        instr->data[i] = this->inputfile.get();

      if (!this->inputfile.good()) {
        invalidate("broken pipe");
        return NULL;
      }
    }

    char e;
    if((e = this->inputfile.get()) != PCL_END_OF_INSTRUCTION) {
      invalidate((format("corrupt instruction terminator at (%08X)") % this->inputfile.tellg()).str());
      return NULL;
    }

    if (expected && !instr->matches(expected, true))
      return NULL;
    else
      return instr;

    //FIXME invalidate?
    if (expected) {
      cerr << "failed to read: " << expected << std::endl;
    }
    return NULL;
  }
};

#endif /* PLOTTER_H_ */

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
#include "HPGL.h"

using std::ios;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::ifstream;
using std::numeric_limits;
using boost::format;

using namespace cimg_library;

class BitmapPlotter {
private:
  BoundingBox *bbox;
  BoundingBox *clip;
  uint32_t width;
  uint32_t height;
  uint8_t *imgbuffer;

public:
  Point penPos;

  // width/height is given in bytes
  // 
  BitmapPlotter(uint32_t width, uint32_t height, BoundingBox *clip = NULL) :
    bbox(new BoundingBox()), clip(clip), width(width), height(height), penPos(0, 0) {
    if (clip != NULL) {
      this->width = clip->min(width, clip->lr.x - clip->ul.x);
      this->height = clip->min(height, clip->lr.y - clip->ul.y);
    }

    this->imgbuffer = new uint8_t[this->width * this->height];
    memset(this->imgbuffer, 0x00, this->width * this->height);
  }

  BitmapPlotter(BoundingBox* clip = NULL) :
    bbox(new BoundingBox()), clip(clip), penPos(0, 0) {
    this->imgbuffer = NULL;
  }

  void move(coord x, coord y) {
    Point p(x, y);
    move(p);
  }

  void move(Point &to) {
    this->penPos = to;
    // Trace::singleton()->logPlotterStat(penPos);
  }
  
  void writeBitmap(uint8_t bitmap) {
    this->fill(bitmap, 1);
  }

  void fill(uint8_t bitmap, int len) {
    int dir = (len < 0) ? -1 : 1;
    Point pos = this->penPos;
    if (this->clip) {
      if (this->penPos.y < this->clip->ul.y || this->penPos.y > this->clip->lr.y) return;
      pos.x -= this->clip->ul.x;
      pos.y -= this->clip->ul.y;
    }
    for (int i=0;i<abs(len);i++) {
      if (this->clip) {
        if ((this->penPos.x + i) < this->clip->ul.x || (this->penPos.x + i) > this->clip->lr.x) return;
      }
      this->imgbuffer[pos.y * this->width + pos.x + i*dir] = bitmap;
      this->bbox->update(this->penPos.x + i*dir, this->penPos.y);
    }
  }

  virtual BoundingBox* getBoundingBox() {
    return bbox;
  }

  virtual CImg<uint8_t>* getCanvas() {
    if(!this->bbox->isValid())
      return NULL;

    Point start(0,0);
    Point size(this->width, this->height);
    if (PclIntConfig::singleton()->autocrop) {
      start.x = this->bbox->ul.x;
      start.y = this->bbox->ul.y;

      size.x = this->bbox->lr.x - this->bbox->ul.x + 1;
      size.y = this->bbox->lr.y - this->bbox->ul.y + 1;
    }
    
    CImg<uint8_t> *image = new CImg<uint8_t>(size.x*8, size.y, 1, 1, 255);
    for (uint32_t y=0;y<size.y;y++) {
      for (uint32_t x=0;x<size.x;x++) {
        uint8_t bitmap = this->imgbuffer[(y + start.y)*this->width + (x + start.x)];
        for (int b=0;b<8;b++) {
          uint8_t val = (bitmap & (0x80 >> b)) ? 0 : 255;
          image->draw_point(x*8 + b, y, &val);
        }
      }
    }
    
    return image;
  }
};

class HPGLPlot {
private:
  ifstream* inputfile;
  bool valid;

public:
  HPGLInstr* currentInstr;

  HPGLPlot(ifstream *infile) :
    inputfile(infile), valid(false), currentInstr(NULL) {
    HPGLInstr* instr = readInstr();
    if(instr != NULL && instr->matches("IN", true))
      valid = true;
  }

  virtual ~HPGLPlot() {
  }

  /*void printSettings(ostream& os) {
    std::map<string, HPGLInstr*>::iterator it;
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
  }*/

  void invalidate(string msg) {
    this->valid = false;
    Trace::singleton()->printBacklog(cerr, "HPGL", msg);
  }

  bool isValid() {
    return valid;
  }

  bool good() {
    return this->valid && this->inputfile->good();
  }

  HPGLInstr* readInstr() {
    HPGLInstr* instr = this->currentInstr = new HPGLInstr(this->inputfile->tellg());

    stringstream ss;
    char c;
    while(isalpha(c = this->inputfile->get()))
      ss << c;

    if(c == PCL_START_OF_INSTRUCTION) {
      this->invalidate("HPGL ended");
      return NULL;
    }

    instr->operation = ss.str();
    ss.str( "" );

    if(c == HPGL_END_OF_INSTRUCTION)
      return instr;
    else
      this->inputfile->unget();

    while(isdigit(c = this->inputfile->get()))
      ss << c;

    instr->parameters[0] = static_cast<int32_t> (strtoll(ss.str().c_str(),
        NULL, 10));

    if (c == HPGL_END_OF_INSTRUCTION)
      return instr;

    if (c == ',') {
      ss.str("");
      while (isdigit(c = this->inputfile->get()))
        ss << c;

      instr->parameters[1] = static_cast<int32_t> (strtoll(ss.str().c_str(),
          NULL, 10));

      if (c != HPGL_END_OF_INSTRUCTION) {
        invalidate("end of instruction expected");
        return NULL;
      }
    } else {
      this->inputfile->unget();
    }

    invalidate("end of instruction expected");
    return NULL;
  }
};


class PclPlot {
private:
  ifstream* inputfile;
  string title;
  const uint16_t bufSize;
  char* buffer;
  off64_t eof;
  bool valid;

  std::map<string, PclInstr*> settings;


  off64_t fillBuffer() {
    off64_t off = this->inputfile->tellg();
    char c;

    if ((c = this->inputfile->get()) == PCL_START_OF_INSTRUCTION) {
      c = this->inputfile->get();
      if (c == '&' || c == '%' || c == '*') {
        buffer[0] = c;
        for (int i = 1; i < (bufSize - 1) && this->inputfile->good(); i++) {
          buffer[i] = c = this->inputfile->get();
          if (!(isalnum(buffer[i]) || buffer[i] == '-')) {
            buffer[i] = '\0';
            this->inputfile->unget();
            return off;
          }
        }
      }
    }
    this->inputfile->unget();
    return eof;
  }

  void readSettings() {
    PclInstr* instr;
    while ((instr = readInstr()) && !instr->matches(PCL_RASTER_START)) {
      settings[(char*) instr] = instr;
    }
    printSettings(cerr);
  }

public:
  PclInstr* currentInstr;

  PclPlot(ifstream *infile) :
    inputfile(infile), bufSize(1024), buffer(
        new char[1024]), eof(numeric_limits<off64_t>::max()), valid(true), currentInstr(NULL) {
    readSettings();
  }

  virtual ~PclPlot() {
  }

  bool isValid() {
    return valid;
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
    Trace::singleton()->printBacklog(cerr, "HPGL", msg);
  }

  bool good() const {
    return this->valid && this->inputfile->good();
  }

  int32_t* readValue(uint8_t& bufferOff) {
    stringstream ss;
    char digit;

    for (; bufferOff < (bufSize - 1) && (isdigit(digit = buffer[bufferOff]) || digit == '-'); ++bufferOff) {
      ss << digit;
    }

    string strval = ss.str();

    if (strval.size() > 0) {
      int32_t* val = new int32_t();
      *val = static_cast<int32_t> (strtoll(strval.c_str(), NULL, 10));
      return val;
    }
    return NULL;
  }

  PclInstr* readInstr(const char * expected = NULL) {
    if(!this->isValid())
      return NULL;

    off64_t fileoff = fillBuffer();

    if(fileoff == eof) {
      invalidate("end of pcl");
      return NULL;
    }

    uint8_t bufferOff = 0;
    PclInstr* instr = new PclInstr(fileoff);
    this->currentInstr = instr;
    instr->type = buffer[bufferOff++];
    instr->prefix = buffer[bufferOff++];
    int32_t * pval = readValue(bufferOff);
    instr->suffix = buffer[bufferOff++];

    if (pval != NULL) {
      instr->value=*pval;
      instr->hasValue = true;
    }

    Trace::singleton()->logInstr(instr);

    if (instr->matches(PCL_END_OF_PASS)) {
      char c;
      for(; bufferOff < bufSize && (c = buffer[bufferOff]) != '\0'; ++bufferOff)
        this->inputfile->unget();

      invalidate("end of pass");
      return NULL;
    } else if (instr->matches(PCL_RLE_DATA)) {
      // copy data part of rle instructions
      instr->data = new uint8_t[instr->value];
      instr->limit = instr->value;
      instr->hasData = true;

      for (int i = 0; i < instr->limit && this->inputfile->good(); i++)
        instr->data[i] = this->inputfile->get();

      if (!this->inputfile->good()) {
        invalidate("broken pipe");
        return NULL;
      }
    }

    if (expected && !instr->matches(expected, true))
      return NULL;
    else
      return instr;

    //FIXME invalidate?
    if (expected)
      cerr << "failed to read: " << expected << std::endl;

    return NULL;
  }
};

enum RtlContext {
  PCL_CONTEXT, HPGL_CONTEXT, NONE
};

PclPlot *watchPclPlot;

class RtlPlot {
private:
  ifstream* inputfile;
  string title;
  PclPlot *currentPclPlot;
  HPGLPlot *currentHPGLPlot;
  bool valid;

  void readRTLIntro() {
    char* buffer = new char[MAGIC_SIZE];
    this->inputfile->read(buffer, MAGIC_SIZE);

    if (memcmp(MAGIC, buffer, MAGIC_SIZE) != 0) {
      invalidate("magic doesn't match");
    } else {
      getline(*this->inputfile, this->title);
      // discard enter pcl
      string enterpcl;
      getline(*this->inputfile, enterpcl);
    }
  }

  void invalidate(string msg) {
    this->valid = false;
    Trace::singleton()->printBacklog(cerr, "RTL", msg);
  }

  bool checkHPGLContext() {
    if (currentHPGLPlot != NULL && currentHPGLPlot->isValid()) {
      return true;
    } else {
      return false;
    }
  }

  bool checkPclContext() {
    if (currentPclPlot != NULL && currentPclPlot->isValid()) {
      return true;
    } else {
      return false;
    }
  }
public:

  RtlPlot(ifstream *infile) :
    inputfile(infile), currentPclPlot(NULL), currentHPGLPlot(NULL), valid(true) {
    readRTLIntro();
  }

  virtual ~RtlPlot() {
  }

  bool isValid() {
    return valid;
  }

  RtlContext getActiveContext() {
    //check for existing valid context plotters
    if(checkPclContext())
        return PCL_CONTEXT;

    if(checkHPGLContext())
        return HPGL_CONTEXT;

    //try to initialize a new context and probe it
    currentPclPlot = new PclPlot(inputfile);
    watchPclPlot = currentPclPlot;
    if(checkPclContext())
      return PCL_CONTEXT;

    currentHPGLPlot = new HPGLPlot(inputfile);
    if(checkHPGLContext())
      return HPGL_CONTEXT;

    invalidate("end of rtl");
    return NONE;
  }

  PclPlot* requestPclPlot() {
    return currentPclPlot;
  }

  HPGLPlot* requestHPGLPlot() {
    return currentHPGLPlot;
  }
};
#endif /* PLOTTER_H_ */

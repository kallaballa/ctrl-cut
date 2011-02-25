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
#include <string>
#include <limits>
#include <map>
#include "CImg.h"

using std::cerr;
using std::endl;
using std::string;
using std::fstream;
using std::ostream;
using std::ios;
using std::map;
using std::numeric_limits;
using namespace cimg_library;

#define PCL_X "*pX"
#define PCL_Y "*pY"
#define PCL_WIDTH "*rS"
#define PCL_HEIGHT "*rT"
#define PCL_RLE "*bW"
#define PCL_PIXEL_LEN "*bA"
#define PCL_END_OF_SETTINGS "*rA"
#define PCL_END_OF_INSTRUCTION 0x1b

char magic[] = { 0x1b, 0x25, 0x2d, 0x31, 0x32, 0x33, 0x34,
        0x35, 0x58, 0x40, 0x50, 0x4a, 0x4c, 0x20, 0x4a, 0x4f, 0x42, 0x20, 0x4e,
        0x41, 0x4d, 0x45, 0x3d };

typedef uint32_t coord;
typedef uint32_t dim;

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

  BoundingBox(coord ul_x=numeric_limits<uint32_t>::max(), coord ul_y=numeric_limits<uint32_t>::max(), coord lr_x=0, coord lr_y=0): ul(ul_x, ul_y), lr(lr_x,lr_y){}

  void update(coord ul_x, coord ul_y, coord lr_x, coord lr_y) {
      this->ul.x = min(this->ul.x, ul_x);
      this->ul.y = min(this->ul.x, ul_y);
      this->lr.x = max(this->lr.x, lr_x);
      this->lr.y = max(this->lr.x, lr_y);
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

class PclInstruction {
public:
  PclInstruction() : keysep('\0'), pos(0), limit(numeric_limits<uint8_t>::max()) {}
  char type;
  char prefix;
  char suffix;
  char keysep; // separate the instr signature from value/data so it may be interpreted as a string and used as key
  int32_t value;
  uint8_t * data;
  uint8_t pos;
  uint8_t limit;

  bool hasNext(bool report=true) {
    bool hasnext = this->pos < this->limit;
    if(!hasnext && report)
        std::cerr << "short read" << std::endl;
    return hasnext;
  }

  uint8_t next(){
      return (uint8_t) *(this->data+this->pos++);
  }

  bool matches(const string& signature) {
    return memcmp(this, signature.c_str(), 3) == 0;
  }
};

class PclFile {
private:
  fstream pclfile;
  const char* title;
  const int buflen;
  char* buffer;
  bool valid;

  bool readPattern(const char * pattern, const int off, const int len) {
    if (this->pclfile.read(buffer, len)) {
      return memcmp((pattern + off), buffer, len) == 0;
    } else
      return false;
  }

  char* readInstrBuffer() {
    for (int i = 0; i < (buflen - 1) && this->pclfile.good(); i++) {
      buffer[i] = this->pclfile.get();
      if (buffer[i] == PCL_END_OF_INSTRUCTION) {
        buffer[i] = '\0';
        return buffer;
      }
    }

    return NULL;
  }

public:
  dim width;
  dim height;
  std::map<string, PclInstruction*> settings;

  PclFile(char* filename) : pclfile(filename, ios::in | ios::binary), buflen(1024), buffer(new char[1024]) {
    this->valid = true;

    if (!readPattern(magic,0, 23))
      invalidate("read magic");

    if (!(this->title = readInstrBuffer()))
      invalidate("read title");

    if (!readInstrBuffer())
      invalidate("skip pcl intro");

    PclInstruction* instr;
    while ((instr = readInstr()) && !instr->matches(PCL_END_OF_SETTINGS)) {
      settings[(char*) instr] = instr;
    }

  //  std::map<string, PclInstruction*>::iterator it;
  //  for (it = settings.begin(); it != settings.end(); it++) {
  //    cerr << "param: " << (*it).start << std::endl;
  // }

    if(isSet(PCL_WIDTH))
      this->width = setting(PCL_WIDTH);
    else {
      this->valid = false;
    }

    if(isSet(PCL_HEIGHT))
      this->height = setting(PCL_HEIGHT);
    else {
      this->valid = false;
    }

  };
  virtual ~PclFile(){};

  bool isSet(string signature) {
    return this->settings.find(signature) != settings.end();
  }

  uint32_t setting(string signature) {
    return this->settings[signature]->value;
  }

  void invalidate(string msg) {
    cerr << "invalid pcl file: " << msg << endl;
    this->valid = false;
  }

  bool good() {
    return this->valid && this->pclfile.good();
  }

  PclInstruction* readInstr(const char * expected=NULL) {
    if(!this->pclfile.good()) {
      cerr << "broken pipe" << std::endl;
      return NULL;
    }

    char* token = readInstrBuffer();
    if (token) {
      PclInstruction* instr = new PclInstruction();
      char * endptr;
      char * cursor = token;
      instr->type = *cursor;
      instr->prefix = *(++cursor);
      instr->value = strtol((++cursor), &endptr, 10);

      if (endptr <= cursor) {
        cerr << "failed to parse value:";

        if(expected)
          cerr << expected;

        cerr << "(" << instr->type << instr->prefix << ")" << std::endl;
        return NULL;
      }

      instr->suffix = *endptr;

      if(expected && !instr->matches(expected)) {
        cerr << "expected: " << expected << " skipping: " << (char*)instr << std::endl;
        return NULL;
      } else {
        if (instr->matches(PCL_RLE)) {
          //copy data part of rle instruction
          instr->data = new uint8_t[instr->value];
          memcpy(instr->data, (++endptr), instr->value);
        }
        return instr;
      }
    }

    if(expected) {
      cerr << "failed to read: " << expected << std::endl;
    }
    return NULL;
  }
};

class RLEInterpreter {

public:
  coord translate_x;
  coord translate_y;

  RLEInterpreter(char* filename, int translate_x=0, int translate_y=0): translate_x(translate_x), translate_y(translate_y), pclfile(filename) {}

  CImg<uint8_t>* render() {
    if(!this->pclfile.good()) {
      this->pclfile.invalidate("corrupt pcl header");
      return NULL;
    }
    PclInstruction *xInstr, *yInstr, *pixlenInstr, *rleInstr;
    CImg<uint8_t>* img = new CImg<uint8_t>(this->pclfile.width, this->pclfile.height, 1, 1, 255);
     do {
       if (!((yInstr = pclfile.readInstr(PCL_Y))
           && (xInstr = pclfile.readInstr(PCL_X))
           && (pixlenInstr = pclfile.readInstr(PCL_PIXEL_LEN))
           && (rleInstr = pclfile.readInstr(PCL_RLE))))
         continue;

       rleInstr->limit = rleInstr->value;
       this->x = translate_x + xInstr->value;
       this->y = translate_y + yInstr->value;
       this->decodedLen = abs(pixlenInstr->value);
       this->reverse = pixlenInstr->value < 0;

       coord pos = 0;
       while ((pos = this->decodeRLE(img, rleInstr, pos))) {}
     } while (this->pclfile.good());

    return img;
  }

  BoundingBox& findBoundingBox() {
    if (!this->pclfile.good()) {
      this->pclfile.invalidate("corrupt pcl header");
      this->bbox.update(0,0,0,0);
    } else {
      PclInstruction *xInstr, *yInstr, *pixlenInstr, *rleInstr;

      do {
        if (!((yInstr = pclfile.readInstr(PCL_Y))
            && (xInstr = pclfile.readInstr(PCL_X))
            && (pixlenInstr = pclfile.readInstr(PCL_PIXEL_LEN))
            && (rleInstr = pclfile.readInstr(PCL_RLE))))
          continue;

        rleInstr->limit = rleInstr->value;
        this->x = this->translate_x + xInstr->value;
        this->y = this->translate_y + yInstr->value;
        this->decodedLen = abs(pixlenInstr->value);

        this->bbox.update(x,y, x + decodedLen, y);
      } while (this->pclfile.good());
    }
    return this->bbox;
  }
private:
  BoundingBox bbox;
  PclFile pclfile;
  uint8_t packedLen;
  dim decodedLen;
  coord x;
  coord y;
  bool reverse;
  bool fill;

  void init(int rl) {
    if(rl > 128) {
      this->fill = true;
      this->packedLen  = 257 - rl;
    }
    else if(rl < 128) {
      this->fill = false;
      this->packedLen  = rl + 1;
    }
    else {
      this->packedLen = 0;
    }
  }

  coord decodeRLE(CImg<uint8_t>* img, PclInstruction* rleInstr, coord pos) {
    if(!rleInstr->hasNext())
      return 0;

    init(rleInstr->next());

    if(this->packedLen == 0)
      return 0;

    uint8_t intensity = 0;

    if (this->fill && rleInstr->hasNext())
      intensity = 255 - rleInstr->next();
    else
      intensity = 0;

    for (int i = 0; i < this->packedLen; ++i) {
      if (!this->fill)
        intensity = rleInstr->hasNext() ? 255 - rleInstr->next() : 0;

      if(!this->reverse)
        (*img)(this->x + this->decodedLen - (pos + i) - 1, this->y, 0) = intensity;
      else
        (*img)(this->x + pos + i, this->y, 0) = intensity;
    }

    return pos + this->packedLen;
  }
};

#endif /* PCLFILE_H_ */

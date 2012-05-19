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

#ifndef PLOT_H_
#define PLOT_H_

#include <assert.h>
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
#include "PclIntConfig.hpp"
#include "HPGL.hpp"

using std::ios;
using std::cin;
using std::cerr;
using std::endl;
using std::string;
using std::ifstream;
using std::numeric_limits;
using boost::format;

class HpglPlot {
private:
  ifstream* inputfile;
  bool valid;

public:
  HpglInstr* currentInstr;

  HpglPlot(ifstream *infile) :
    inputfile(infile), valid(false), currentInstr(NULL) {
    HpglInstr* instr = readInstr();
    if(instr != NULL && instr->matches(HPGL_INIT, true))
      valid = true;
  }

  virtual ~HpglPlot() {
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

  HpglInstr* readInstr() {
    HpglInstr* instr = this->currentInstr = new HpglInstr(this->inputfile->tellg());

    stringstream ss;
    char c;
    while(isalpha(c = this->inputfile->get()))
      ss << c;

    if (ss.str().length() == 0 && isdigit(c)) {
      instr->operation = HPGL_MOVE;
    } else {
      if (c == PCL_START_OF_INSTRUCTION) {
        this->invalidate("HPGL ended");
        this->inputfile->unget();
        return NULL;
      }

      instr->operation = ss.str();
      ss.str("");
    }

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

      if (c != HPGL_END_OF_INSTRUCTION && c != ',') {
        invalidate("end of instruction expected");
        return NULL;
      } else {
        return instr;
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

          if (isupper(buffer[i])) {

            if(i + 1 >= bufSize)
              assert("Instruction exceeded pclplot buffer");
            else
              buffer[i + 1] = '\0';

            return off;
          }
        }
      }
    }
    this->inputfile->unget();
    return eof;
  }

public:
  PclInstr* currentInstr;

  PclPlot(ifstream *infile) :
    inputfile(infile), bufSize(1024), buffer(
        new char[1024]), eof(numeric_limits<off64_t>::max()), valid(true), currentInstr(NULL) {
    this->readSettings();
  }

  virtual ~PclPlot() {
  }

  bool isValid() {
    return valid;
  }

  void readSettings() {
    PclInstr* instr;
    while ((instr = readInstr()) && !instr->matches(PCL_RASTER_START)) {
      settings[(char*) instr] = instr;
    }
    printSettings(cerr);
  }

  void printSettings(ostream& os) {
    if(PclIntConfig::singleton()->debugLevel >= LVL_DEBUG && settings.size() > 0) {
      std::map<string, PclInstr*>::iterator it;

      os << "=== settings: " << endl;
      for (it = settings.begin(); it != settings.end(); it++) {
        os << "\t" << *((*it).second) << endl;
      }
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
    Trace::singleton()->printBacklog(cerr, "PCL", msg);
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
  }
};

enum RtlContext {
  PCL_CONTEXT, HPGL_CONTEXT, NONE
};

class RtlPlot {
private:
  ifstream* inputfile;
  string title;
  PclPlot *currentPclPlot;
  HpglPlot *currentHpglPlot;
  bool valid;
  uint32_t width;
  uint32_t height;
  uint16_t resolution;

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
    if (currentHpglPlot != NULL && currentHpglPlot->isValid()) {
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
    inputfile(infile), currentPclPlot(NULL), currentHpglPlot(NULL), valid(true) {
    readRTLIntro();
    currentPclPlot = new PclPlot(infile);

    if (currentPclPlot->require(PCL_WIDTH) && currentPclPlot->require(PCL_HEIGHT)
        && currentPclPlot->require(PCL_PRINT_RESOLUTION)) {
      this->width = currentPclPlot->setting(PCL_WIDTH);
      this->height = currentPclPlot->setting(PCL_HEIGHT);
      this->resolution = currentPclPlot->setting(PCL_PRINT_RESOLUTION);
    } else
      currentPclPlot->invalidate("can't find plot dimensions");
  }

  virtual ~RtlPlot() {
  }

  bool isValid() {
    return valid;
  }

  uint32_t getWidth() {
    return width;
  }

  uint32_t getHeight() {
    return height;
  }

  uint32_t getResolution() {
    return resolution;
  }

  RtlContext getActiveContext() {
    //check for existing valid context plotters
    if(checkPclContext())
        return PCL_CONTEXT;

    if(checkHPGLContext())
        return HPGL_CONTEXT;

    //try to initialize a new context and probe it
    currentPclPlot = new PclPlot(inputfile);
    if(checkPclContext())
      return PCL_CONTEXT;

    currentHpglPlot = new HpglPlot(inputfile);
    if(checkHPGLContext())
      return HPGL_CONTEXT;

    //try to initialize a new context and probe it
    currentPclPlot = new PclPlot(inputfile);
    if(checkPclContext())
      return PCL_CONTEXT;

    currentHpglPlot = new HpglPlot(inputfile);
    if(checkHPGLContext())
      return HPGL_CONTEXT;

    invalidate("end of rtl");
    return NONE;
  }

  PclPlot* requestPclPlot() {
    return currentPclPlot;
  }

  HpglPlot* requestHPGLPlot() {
    return currentHpglPlot;
  }
};
#endif /* PLOT_H_ */

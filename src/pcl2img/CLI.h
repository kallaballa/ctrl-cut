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

#include <string>
#include <sstream>
#include <stdlib.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "Plot.h"
#include "CImg.h"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::stringstream;
using boost::lexical_cast;

using namespace cimg_library;

#ifndef CLI_H_
#define CLI_H_

class Debugger {
private:
  PclPlotter* plotter;
  boost::mutex ia_mutex;
  volatile bool interactive;
  CImgDisplay* canvas_disp;
  bool anim;
  list<off64_t> breakpoints;
  list<string> signatures;
  string lastCliCmd[2];

  void exec(string cmd, string param) {
    if (cmd.compare("run") == 0) {
      this->consume();
    } else if (cmd.compare("show") == 0) {
      this->displayCanvas();
    } else if (param) {
      if (cmd.compare("break") == 0) {
        off64_t off = strtoll(param.c_str(), NULL, 16);
        if (off > 0)
          breakpoints.push_back(off);
      } else if (cmd.compare("step") == 0) {
        this->waitSteps(strtol(param.c_str(), NULL, 10));
      } else if (cmd.compare("find") == 0) {
        this->signatures.push_back(param);
        this->setInteractive(false);
        this->consume();
      } else if (cmd.compare("anim") == 0) {
        if (param.compare("on") == 0)
          this->anim = true;
        else if (param.compare("off") == 0)
          this->anim = false;
      } else if (cmd.compare("dump") == 0) {
        this->dumpCanvas(param.c_str());
        cerr << "dumped: " << param << endl;
      }
    } else {
      cerr << "<< invalid command" << endl;
      return;
    }
    lastCliCmd[0] = cmd;
    lastCliCmd[1] = param;
  }
  void consume() {
    this->setInteractive(false);
    while (!isInteractive())
      this->step_barrier.wait();
  }

  void waitSteps(uint32_t steps = 1) {
    while (isInteractive() && steps--) {
      this->step_barrier.wait();
    }
  }

  void checkBreakpoints(PclInstr *instr) {
    if (breakpoints.size() > 0 && instr) {
      list<off64_t>::iterator it;
      off64_t bp = numeric_limits<off64_t>::max();
      for (it = breakpoints.begin(); it != breakpoints.end(); it++) {
        bp = *it;
        if (instr->file_off >= bp) {
          cerr << (format("=== breakpoint (%08X)") % bp) << endl;
          setInteractive(true);
          this->step_barrier.wait();
          it = breakpoints.erase(it);
          break;
        }
      }
    }
  }

  void checkSignatures(PclInstr *instr) {
    if (signatures.size() > 0 && instr) {
      list<string>::iterator it;
      for (it = signatures.begin(); it != signatures.end(); it++) {
        if (instr->matches(*it)) {
          cerr << "=== found " << (char *) instr << endl;
          setInteractive(true);
          this->step_barrier.wait();
           break;
        }
      }
    }
  }

  void checkStepBarrier() {
    if (isInteractive())
      this->step_barrier.wait();
  }


public:
  boost::barrier step_barrier;
  static Debugger* instance;
  static void create(PclPlotter* plotter);

  Debugger(PclPlotter* plotter) :
    plotter(plotter), canvas_disp(NULL), anim(false), step_barrier(2) {
  }

  void loop() {
    string line;

    while (this->isInteractive() && cin) {
      getline(cin, line);
      stringstream ss(line);
      string cmd;
      string param;
      if(line.length() > 0) {
        ss >> cmd;
        ss >> param;
        exec(cmd,param);
      } else
        exec(lastCliCmd[0], lastCliCmd[1]);
    }
  }

  bool isInteractive() {
    boost::mutex::scoped_lock ia_lock(ia_mutex);
    return interactive;
  }

  void setInteractive(bool i) {
    boost::mutex::scoped_lock ia_lock(ia_mutex);
    //FIXME inclomplete sync: old loop still might be reading from cin
    if (!this->interactive && i)
      boost::thread cli_thrd(&Debugger::loop, this->instance);
    this->interactive = i;
  }

  void announce(PclInstr* instr) {
    checkStepBarrier();
    checkBreakpoints(instr);
    checkSignatures(instr);
  }

  void animate() {
    if (anim)
      this->updateCanvas();
  }

  void dumpCanvas(const char* filename) {
    plotter->getCanvas()->save(filename);
  }

  void updateCanvas() {
    if (canvas_disp) {
      canvas_disp->display(*plotter->getCanvas());
      canvas_disp->paint();
    }
  }

  void displayCanvas() {
    if (!canvas_disp) {
      canvas_disp = new CImgDisplay(*plotter->getCanvas(), "Plot");
    }
    updateCanvas();
  }
};

Debugger* Debugger::instance = NULL;
void Debugger::create(PclPlotter* plotter) {
  if(instance == NULL)
    instance = new Debugger(plotter);
}

#endif /* CLI_H_ */
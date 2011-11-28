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
#ifndef CLI_H_
#define CLI_H_

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

class Debugger {
public:
  boost::mutex ia_mutex;
  void* vplotter;
  volatile bool interactive;
  CImgDisplay* canvas_disp;
  bool autoupdate;
  list<off64_t> breakpoints;
  string find;
  string lastCliCmd[2];
  boost::thread* cli_thrd;
  static Debugger* instance;
  boost::barrier step_barrier;

  void exec(string cmd, string param) {
    if (cmd.compare("help") == 0) {
     cerr << "run               continue processing the plot" << endl\
          << "break <hexoff>    set a breakpoint at the given address" << endl\
          << "step  <num>       process the given number of operations" << endl\
          << "find <instr>      find the next occurence of the given instruction" << endl\
          << "update <on/off>   without parameter just update the screen with the current plot stat" << endl\
          << "                  passing 'on' enables live updating the screen" << endl\
          << "                  passing 'off' disables live updating the screen" << endl\
          << "dump <filename>   dump the current state of the plot rendering to file" << endl;
    } else if (cmd.compare("run") == 0) {
      this->consume();
    } else {
      if (cmd.compare("break") == 0) {
        off64_t off = strtoll(param.c_str(), NULL, 16);
        if (off > 0) {
          breakpoints.push_back(off);
          cerr << "=== seeking: " << off << endl;
        } else
        cerr << "=== invalid offset: " << off << endl;
      } else if (cmd.compare("step") == 0) {
        this->waitSteps(strtol(param.c_str(), NULL, 10));
      } else if (cmd.compare("find") == 0) {
        find = param;
        cerr << "=== searching: " << find << endl;
        this->setInteractive(false);
        this->consume();
        find = "";
      } else if (cmd.compare("update") == 0) {
        if (param.compare("on") == 0) {
          this->autoupdate = true;
          cerr << "=== auto update on" << endl;
        }
        else if (param.compare("off") == 0) {
          this->autoupdate = false;
          cerr << "=== auto update off" << endl;
        }
      }
      //   else if(vplotter != NULL)
      //     vplotter->getCanvas()->update();
      // } else if (cmd.compare("dump") == 0 && vplotter != NULL) {
      //   vplotter->getCanvas()->dump(param.c_str());
      //   cerr << "=== dumped: " << param << endl;
      // }
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
//          break;
        }
      }
    }
  }

  void checkBreakpoints(HpglInstr *instr) {
    if (breakpoints.size() > 0 && instr) {
      list<off64_t>::iterator it;
      off64_t bp = numeric_limits<off64_t>::max();
      for (it = breakpoints.begin(); it != breakpoints.end(); it++) {
        bp = *it;
        if (instr->file_off >= bp) {
          cerr << (format("=== breakpoint (%08X)") % bp) << endl;
          setInteractive(true);
          // if(vplotter != NULL)
          //   vplotter->getCanvas()->update();
          this->step_barrier.wait();
          it = breakpoints.erase(it);
          break;
        }
      }
    }
  }

  void checkSignatures(PclInstr *instr) {
    if (find.length() > 0 && instr && instr->matches(find)) {
      cerr << "=== found " << (char *) instr << endl;
      setInteractive(true);
      this->step_barrier.wait();
    }
  }

  void checkSignatures(HpglInstr *instr) {
    if (find.length() > 0 && instr && instr->matches(find)) {
      cerr << "=== found " << instr->operation << endl;
      // if(vplotter != NULL)
      //   vplotter->getCanvas()->update();
      setInteractive(true);
      this->step_barrier.wait();
    }
  }

  void checkStepBarrier() {
    if (isInteractive())
      this->step_barrier.wait();
    // if(this->autoupdate && vplotter != NULL)
    //   vplotter->getCanvas()->update();
  }


public:
  static void create(void *vplotter = NULL);
  static Debugger* getInstance();

  Debugger(void* vplotter) :
    vplotter(vplotter), interactive(false), canvas_disp(NULL), autoupdate(false), cli_thrd(NULL), step_barrier(2) {
  }

  virtual void loop() {
    string line;

    while (cin) {
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

  virtual bool isInteractive() {
    boost::mutex::scoped_lock ia_lock(ia_mutex);
    return interactive;
  }

  virtual void setInteractive(bool i) {
    boost::mutex::scoped_lock ia_lock(ia_mutex);
    //FIXME inclomplete sync: old loop still might be reading from cin
    if (!this->interactive && i && this->cli_thrd == NULL)
     this->cli_thrd = new boost::thread(&Debugger::loop, this->instance);

    this->interactive = i;
  }

  virtual void announce(PclInstr* instr) {
    checkStepBarrier();
    checkBreakpoints(instr);
    checkSignatures(instr);
  }

  virtual void announce(HpglInstr* instr) {
    checkStepBarrier();
    checkBreakpoints(instr);
    checkSignatures(instr);
  }
};

Debugger* Debugger::instance = NULL;
class VectorPlotter;
void Debugger::create(void *vplotter) {
  if(instance == NULL)
    instance = new Debugger((void *)vplotter);
}

Debugger* Debugger::getInstance() {
  return instance;
}

#endif /* CLI_H_ */

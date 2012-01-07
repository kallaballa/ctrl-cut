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

#ifndef CUTMODEL_H_
#define CUTMODEL_H_

#include <string>
#include <stdio.h>

#include "config/DocumentSettings.hpp"
#include "config/CutSettings.hpp"
#include "cut/geom/Route.hpp"
#include "cut/geom/sink/AddSink.hpp"


template<
template<typename,typename> class Tcontainer = std::vector,
template<typename> class Tallocator = std::allocator
>
class CutImpl : public RouteImpl<Tcontainer, Tallocator> {
public:
  typedef RouteImpl<Tcontainer, Tallocator> Route_t;
  CutImpl(DocumentSettings& parentSettings) :
    Route_t(parentSettings)
  { }

  CutImpl(const CutSettings& settings) :
    Route_t(settings)
  { }

  //shallow copy
  CutImpl(const CutImpl& other) :
    Route_t(other) {
  }

  ~CutImpl() {
    this->clear();
  }

  /*!
   Loads vector data from EPS/Ghostscript output
   */
  bool load(std::istream &input) {
    std::string line;
    char first;
    int power, x, y;
    int lx = 0, ly = 0;
    int mx = 0, my = 0;
    LOG_INFO_STR("Load vector data");
    int segmentCnt = 0;
    AddSink<CutImpl> sink(*this);

    while (std::getline(input, line)) {
      std::cerr << line << std::endl;
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
            segmentCnt++;
            *sink++ = Segment(Point(lx,ly),Point(mx,my));
          }
          break;
        case 'P': // power
          if (sscanf(line.c_str() + 1, "%d", &x) == 1) {
            // FIXME: While testing, ignore the strange color-intensity-is-power convension
            //          power = x;
            power = -1;
          }
          break;
        case 'L': // line to
          if (sscanf(line.c_str() + 1, "%d,%d", &y, &x) == 2) {
            segmentCnt++;
            *sink++ = Segment(Point(lx,ly),Point(x,y));
            lx = x;
            ly = y;
          }
          break;
        }
      }
    }

    LOG_DEBUG_MSG("loaded segments", segmentCnt);

    if (this->empty()) {
      return false;
    }

    return true;
  }

  /*!
   Loads vector data from EPS/Ghostscript output from the given file
   */
  bool load(const std::string &filename) {
    std::ifstream infile(filename.c_str(), std::ios_base::in);
    return this->load(infile);
  }

  CutImpl make() const {
    return CutImpl(this->settings);
  }

protected:
  /* REFACTOR
  uint64_t clipped;
  uint64_t zerolength;
  */
};

typedef CutImpl<std::vector, std::allocator> CutModel;
#endif /* CUTMODEL_H_ */

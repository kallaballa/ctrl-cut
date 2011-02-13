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

#ifndef POLYLINE_H_
#define POLYLINE_H_

#include "stddef.h"
#include "climits"
#include "iostream"
#include <cmath>
#include "raster/DownSample.h"
#include "VTypes.h"

class Polyline : public std::vector<Edge *> {
public:
  Polyline();
  virtual ~Polyline(){}

  void add(Edge* ls);
  void remove(Edge* ls);
  bool contains(Edge* ls);

  Edge *findLeftmostClockwise();
  BBox* getBoundingBox();

  int size() { return this->std::vector<Edge *>::size(); }
  Edge* front() { return this->std::vector<Edge *>::front(); }
  Edge* back() { return this->std::vector<Edge *>::back(); }
  Polyline::iterator begin() { return this->std::vector<Edge *>::begin(); }
  Polyline::iterator end() { return this->std::vector<Edge *>::end(); }
  Polyline::iterator find(Edge* e) {
    for (Polyline::iterator it = this->begin(); it != this->end(); it++) {
      if (*it == e)
        return it;
    }
    return (Polyline::iterator)NULL;
  }

  friend std::ostream& operator <<(std::ostream &os, Polyline &pl);

private:
  static int cnt;
  int id;

  BBox bb;
};

#endif /* POLYGON_H_ */

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

#include "util/2D.h"
#include <vector>

class Polyline
{
public:
  typedef std::vector<class Edge*> EdgeVector;
  typedef EdgeVector::iterator iterator;
  typedef EdgeVector::const_iterator const_iterator;

  Polyline();
  virtual ~Polyline(){}

  iterator begin() { return this->edges.begin(); }
  const_iterator begin() const  { return this->edges.begin(); }
  iterator end() { return this->edges.end(); }
  const_iterator end() const  { return this->edges.end(); }
  size_t size() const { return this->edges.size(); }

  bool isClosed() const;
  void add(Edge* ls);
  void remove(Edge* ls);
  bool contains(Edge* ls);

  Edge *findLeftmostClockwise();
  BBox* getBoundingBox();

  friend std::ostream& operator <<(std::ostream &os, Polyline &pl);

private:
  static int cnt;
  int id;

  BBox bb;

  EdgeVector edges;
};

#endif /* POLYGON_H_ */

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
#ifndef CUT_H_
#define CUT_H_

#include <iostream>
#include <vector>
#include "Mesh.h"

class Cut
{
public:
  typedef std::vector<class Polyline*> PolylineVector;
  typedef PolylineVector::iterator iterator;
  typedef PolylineVector::const_iterator const_iterator;

  Cut() : clipped(false) {}
  virtual ~Cut() {}

  iterator begin() { return this->polylines.begin(); }
  const_iterator begin() const  { return this->polylines.begin(); }
  iterator end() { return this->polylines.end(); }
  const_iterator end() const  { return this->polylines.end(); }
  size_t size() const { return this->polylines.size(); }
  void swap(PolylineVector &v) { this->polylines.swap(v); }

  Mesh &getMesh() { return this->mesh; }
  const Mesh &getMesh() const { return this->mesh; }

  static Cut* load(const std::string &filename);
  static Cut* load(std::istream &input);

  void add(Polyline* ls);
  void remove(Polyline* ls);
  bool contains(Polyline* ls) const;

  bool wasClipped() const {
    return this->clipped;
  }
  void xml(const std::string &s) const;

private:
  bool clipped;
  Mesh mesh;

  PolylineVector polylines;
};

std::ostream &operator<<(std::ostream &os, const Cut &cut);

#endif

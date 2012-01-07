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

#ifndef ENGRAVE_H_
#define ENGRAVE_H_

#include "cut/geom/Geometry.hpp"
#include "image/MMapMatrix.hpp"
#include "config/DocumentSettings.hpp"
#include "config/EngraveSettings.hpp"
#include "engrave/dither/Dither.hpp"
#include "image/PPMFile.hpp"
#include <list>

template<typename Timage>
class EngravingImpl
{
private:
  typedef std::list<Timage> ImageList;
  ImageList images;

public:
  typedef typename ImageList::iterator iterator;
  typedef typename ImageList::const_iterator const_iterator;

  EngraveSettings settings;

  EngravingImpl(DocumentSettings& parentSettings)
  : settings(parentSettings)
  {}

  EngravingImpl(const EngraveSettings& settings)
  : settings(settings) {}


  void load(const string& filename) {
    std::string suffix = filename.substr(filename.rfind(".") + 1);
    const BitmapImage& newBitmap = loadpbm(filename);
    this->settings.put(EngraveSettings::EPOS, Point(392, 516));
    this->push_back(newBitmap);
  }

  ~EngravingImpl() {
    for(iterator it = begin(); it != end(); it++) {
      delete (*it).data();
    }
  }

  iterator begin() {
    return this->images.begin();
  }
  const_iterator begin() const {
    return this->images.begin();
  }
  iterator end()  {
    return this->images.end();
  }
  const_iterator end() const {
    return this->images.end();
  }
  typename iterator::reference front() {
    return this->images.front();
  }
  typename iterator::reference back() {
    return this->images.back();
  }
  typename const_iterator::reference front() const {
    return this->images.front();
  }
  typename const_iterator::reference back() const {
    return this->images.back();
  }

  void clear() {
    images.clear();
  }

  void insert(iterator pos, const Timage& image) {
    images.insert(pos, image);
  }

  void erase(iterator pos) {
    images.erase(pos);
  }

  void push_front(const Timage& image) {
    images.push_front(image);
  }

  void push_back(const Timage& image) {
    images.push_back(image);
  }
};

typedef EngravingImpl<BitmapImage> Engraving;
#endif

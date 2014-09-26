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
#include <Magick++.h>


template<typename Timage>
class EngravingImpl
{
private:
  Timage image;

public:
  EngraveSettings settings;

  EngravingImpl(DocumentSettings& parentSettings)
  : settings(parentSettings)
  {}

  EngravingImpl(const EngraveSettings& settings)
  : settings(settings) {}


  void load(const string& filename) {
    std::string suffix = filename.substr(filename.rfind(".") + 1);
    this->image = loadpbm(filename);
  }

  ~EngravingImpl() {
     delete image.data();
  }

  void setImage(const Timage& image) {
    this->image = image;
  }

  const Timage& getImage() const {
    return image;
  }

  template<typename T, typename V>
  void put(const Settings::Key<T>& key, V value) {
    settings.put(key,value);
  }

  template<typename T>
  const T get(const Settings::Key<T>& key) const {
    return settings.get(key);
  }

  bool isAllocated() {
    return this->image.isAllocated();
  }

  void toJson(std::ostream& os) const {
    GrayscaleImage img = this->getImage();
    Coord_t height = img.height() - 1;

    Magick::Blob rawblob(img.data(), (img.rowstride()) * height);
    Magick::Blob pngblob;
    Magick::Image image(rawblob, Magick::Geometry((img.rowstride()), height), 8, "GRAY");
    image.magick( "PNG" );
    image.write(&pngblob);

    os << "{ \"settings\":" << std::endl;
    this->settings.toJson(os);
    os << "," << std::endl << "\"image\": \"" << pngblob.base64() << "\" }" << std::endl;
  }
};

typedef EngravingImpl<GrayscaleImage> Engraving;
#endif

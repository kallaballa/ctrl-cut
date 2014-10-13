/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
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
#include <memory>
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

  ~EngravingImpl() {
    if(image.isAllocated())
      free(image.data());
  }

  void load(const string& filename) {
    std::string suffix = filename.substr(filename.rfind(".") + 1);
    this->image = loadpbm(filename);
  }

  void setImage(const Timage& image) {
    this->image = image;
  }

  const Timage& getImage() const {
    return image;
  }

  EngravingImpl<Timage>* clone() const {
    EngravingImpl<Timage>* clone = new EngravingImpl<Timage>(this->settings);
    clone->setImage(this->getImage().clone());
    return clone;
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
typedef std::shared_ptr<Engraving> EngravingPtr;

#endif

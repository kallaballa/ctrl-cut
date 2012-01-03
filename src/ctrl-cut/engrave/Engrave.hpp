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
#include "config/EngraveSettings.hpp"
#include "engrave/dither/Dither.hpp"

class Document;

class Engraving
{
public:
  EngraveSettings settings;

  Engraving(AbstractImage& sourceImage, Document& doc);
  Engraving(const std::string&filename, Document& doc);

  virtual ~Engraving() {}

  AbstractImage& getSourceImage() { return *this->sourceImage; }
  AbstractImage& getProcessedImage() {
    //copy the source if no processing took place
    if(!this->processedImage) {
      this->processedImage = this->sourceImage->copy(Rectangle(0,0,this->sourceImage->width(), this->sourceImage->height()));
    }

    return *this->processedImage;
  }

  void dither() {
    GrayscaleImage *gsimage =  dynamic_cast<GrayscaleImage*>(this->sourceImage);
    if (gsimage) {
      EngraveSettings::Dithering dithering = this->settings.get(EngraveSettings::DITHERING);
      Dither& dither = Dither::create(*gsimage, dithering);
      this->processedImage = &dither.dither();
    } else {
      this->processedImage = this->sourceImage;
    }
  }
private:
  AbstractImage* sourceImage;
  AbstractImage *processedImage;
};

#endif

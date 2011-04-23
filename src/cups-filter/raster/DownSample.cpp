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

#include "DownSample.h"

DownSample::DownSample(const Point2D &p_ul, uint16_t res_x, uint16_t res_y,
                       uint16_t pixel_width, uint16_t pixel_height, uint16_t tolerance) {
  this->res_x = res_x;
  this->res_y = res_y;

  this->pixel_width = pixel_width;
  this->pixel_height = pixel_height;
  this->tolerance = tolerance;

  this->boundingBox.ul = p_ul;
  this->boundingBox.lr[0] = p_ul[0] + pixel_width;
  this->boundingBox.lr[1] = p_ul[1] + pixel_height;

  this->image = NULL;
}

DownSample::DownSample(BBox* bbox, uint16_t res_x, uint16_t res_y,
                       uint16_t pixel_width, uint16_t pixel_height, uint16_t tolerance) {
  this->res_x = res_x;
  this->res_y = res_y;

  this->pixel_width = pixel_width;
  this->pixel_height = pixel_height;
  this->tolerance = tolerance;

  this->boundingBox.ul = bbox->ul;
  this->boundingBox.lr[0] = bbox->ul[0] + pixel_width;
  this->boundingBox.lr[1] = bbox->ul[1] + pixel_height;
  this->image = NULL;
}


bool DownSample::sample(const Point2D &p) {
  if (!boundingBox.inside(p, pixel_width * tolerance, pixel_height * tolerance))
    return false;
  else {
    uint16_t cx = p[0] / pixel_width;
    uint16_t cy = p[1] / pixel_height;

    //TODO figure appropriate growth rate
    if (image == NULL || cx >= this->res_x || cy >= this->res_y) {
      uint16_t ncx = std::max(this->res_x, (cx));
      uint16_t ncy = std::max(this->res_y, (cy));
      resize(ncx * 2, ncy * 2);
    }

    PixelBox* pb = (*image)[cx][cy];

    if (pb == NULL) {
      uint16_t ul_x = cx * pixel_width;
      uint16_t ul_y = cy * pixel_height;
      uint16_t lr_x = ul_x + pixel_width;
      uint16_t lr_y = ul_y + pixel_height;
      pb = new PixelBox(ul_x, ul_y, lr_x, lr_y);
      (*image)[cx][cy] = pb;
    }

    boundingBox.extendBy(*pb);
    pixelBoxes.push_back(pb);
    return true;
  }
}

void DownSample::resize(uint16_t num_x, uint16_t num_y) {
  uint16_t offset = 0;
  if (image == NULL)
    image = new PB_Image(boost::extents[num_x][num_y]);
  else {
    offset = this->res_x * this->res_y;
    image->resize(boost::extents[num_x][num_y]);
  }

  std::fill(image->data() + offset, image->data() + image->num_elements(),
            (PixelBox*) NULL);

  this->res_x = num_x;
  this->res_y = num_y;
}

std::list<PixelBox*> DownSample::getPixels() {
  return pixelBoxes;
}

BBox DownSample::getBoundingBox() {
  return this->boundingBox;
}

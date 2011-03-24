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

#include <boost/format.hpp>
#include <math.h>
#include "PclEncoder.h"

using boost::format;

PclEncoder::PclEncoder(LaserConfig* lconf) : lconf(lconf) {
}

PclEncoder::~PclEncoder() {

}

void PclEncoder::encode(Raster* raster, ostream& out) {
  LOG_DEBUG_MSG("Encode raster", raster->tiles.size());

  // Raster Orientation
  out << format(R_ORIENTATION) % 0;
  // Raster power
  out << format(R_POWER) % 100;

  // Raster speed
  out << format(R_SPEED) % lconf->raster_speed;

  out << PCL_UNKNOWN_BLAFOO3;
  out << format(R_HEIGHT) % ((lconf->height * lconf->resolution)
      / POINTS_PER_INCH);
  out << format(R_WIDTH) % ((lconf->width * lconf->resolution)
      / POINTS_PER_INCH);
  // Raster compression
  int compressionLevel = 2;

  out << format(R_COMPRESSION) % compressionLevel;
  // Raster direction (1 = up)
  out << R_DIRECTION_UP;
  // start at current position
  out << R_START_AT_POS;

  list<AbstractImage*>::iterator it;
  for (it = raster->tiles.begin(); it != raster->tiles.end(); it++) {
    BitmapImage *bitmap = dynamic_cast<BitmapImage*>(*it);
    if (!bitmap) {
      LOG_FATAL_STR("Only bitmaps supported at the moment");
    }
    assert(bitmap);
    if (bitmap) encodeBitmapTile(bitmap, out);
  }
  out << "\e*rC"; // end raster
  out << "\26" << "\4"; // some end of file markers
}

void PclEncoder::averageXSequence(GrayscaleImage *img, int fromX, int toX, int y, Pixel<uint8_t>& p){
  float cumm = 0;

  for (int x = fromX; x < toX; x++){
    img->readPixel(x, y, p);
    cumm += p.i;
  }
  cumm = cumm / (toX - fromX);

  if(cumm < 255 && cumm > 254)
    cumm = 254;

  p.i = cumm;
}

void PclEncoder::encodeBitmapTile(BitmapImage* tile, ostream& out) {

  int repeat = this->lconf->raster_repeat;
  while (repeat--) {
    int width = tile->width() / 8; // width in bytes
    int height = tile->height();

    // raster (basic)
    int y;
    bool dir = false;

    for (y = height - 1; y >= 0; y--) {
      uint8_t *scanline = (uint8_t *)tile->data() + y*tile->rowstride(); // Pointer to a scanline

      int l;

      // find left/right of data (dir==0 ? left : right)
      for (l = 0; l < width && (scanline[l] == 0xff); l++) { }

      if (l < width) {
        // a line to print
        int r;
        int n;
        int packsize = width * 5 / 4 + 1;
        char pack[packsize];
        // find left/right of data (dir==0 ? right : left )
        for (r = width - 1; r > l && (scanline[r] == 0xff); r--) { }
        r++;

        out << format(PCL_POS_Y) % (tile->yPos() + lconf->basey + y);
        out << format(PCL_POS_X) % (tile->xPos() + lconf->basex + (l * 8));

        if (dir) {
          //reverse scan line
          out << format(R_ROW_UNPACKED_BYTES) % (-(r - l));
          for (n = 0; n < (r - l) / 2; n++) {
            uint8_t t = scanline[l + n];
            scanline[l + n] = scanline[r - n - 1];
            scanline[r - n - 1] = t;
          }
        } else {
          out << format(R_ROW_UNPACKED_BYTES) % (r - l);
        }
        dir = !dir;
        // pack
        n = 0;
        while (l < r) {
          int p;
          // find run length
          for (p = l; p < r && p < l + 128 && scanline[p] == scanline[l]; p++) { }
          if (p - l >= 2) {
            // run length
            pack[n++] = 257 - (p - l);
            pack[n++] = ~(scanline[l]);
            l = p;
          } else {
            for (p = l; p < r && p < l + 127 && (p + 1 == r || scanline[p] != scanline[p + 1]); p++) { }
            // copy
            pack[n++] = p - l - 1;
            while (l < p) {
              pack[n++] = ~(scanline[l++]);
            }
          }
        }
        assert(n<=packsize);

        out << format(R_ROW_PACKED_BYTES) % ((n + 7) / 8 * 8); // Always write a multiple of 8 bytes

        for (int i = 0; i < n; i++) {
          out << pack[i];
        }

        while (n & 7) {
          n++;
          out << "\x80";
        }
      }
    }
  }
}

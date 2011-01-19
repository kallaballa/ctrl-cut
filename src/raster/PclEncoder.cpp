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

#include "PclEncoder.h"
#include "boost/format.hpp"
using boost::format;

PclEncoder::PclEncoder(LaserConfig *lconf) {
  this->lconf = lconf;
}

PclEncoder::~PclEncoder() {
}

void PclEncoder::encode(Raster* raster, ostream& out) {
  LOG_DEBUG_MSG("Encode raster", raster->tiles.size());
  list<Image*>::iterator it;
  // Raster Orientation
  out << format(R_ORIENTATION) % 0;
  // Raster power
  out << format(R_POWER) % ((lconf->raster_mode == 'c' || lconf->raster_mode
      == 'g') ? 100 : lconf->raster_power);

  out << PCL_UNKNOWN_BLAFOO3;
  // Raster speed
  out << format(R_SPEED) % lconf->raster_speed;
  out << format(R_HEIGHT) % (lconf->height * lconf->y_repeat);
  out << format(R_WIDTH) % (lconf->width * lconf->x_repeat);
  // Raster compression
  int compressionLevel = 2;
  if (lconf->raster_mode == 'c' || lconf->raster_mode == 'g')
    compressionLevel = 7;

  out << format(R_COMPRESSION) % compressionLevel;
  // Raster direction (1 = up)
  out << R_DIRECTION_UP;
  // start at current position
  out << R_START_AT_POS;

  for (it = raster->tiles.begin(); it != raster->tiles.end(); it++) {
    encodeTile(*it, out);
  }
  out << "\e*rC"; // end raster
  out << "\26" << "\4"; // some end of file markers
}

void PclEncoder::encodeTile(Image* tile, ostream& out) {
  int h;
  int w;
  int offx;
  int offy;
  int repeat;

  repeat = this->lconf->raster_repeat;
  while (repeat--) {
    w = tile->width();
    h = tile->height();

    char buf[w];
    float power_scale = lconf->raster_power / (float) 255;

    for (offx = w * (lconf->x_repeat - 1); offx >= 0; offx -= w) {
      for (offy = h * (lconf->y_repeat - 1); offy >= 0; offy -= h) {
        // raster (basic)
        int y;
        char dir = 0;

        for (y = h - 1; y >= 0; y--) {
          int l;

          for (int x = 0; x < w; x++) {
            Pixel<uint8_t>* p = tile->pixel(Point2D(x, y));
            // invert intensity (black -> 255, white -> 0) and apply raster power scale
            buf[x] = (uint8_t) (255 - p->intensity()) * power_scale;
            delete p;
          }

          // find left/right of data
          for (l = 0; l < h && !buf[l]; l++) {
          }

          if (l < h) {
            // a line to print
            int r;
            int n;
            char pack[sizeof(buf) * 5 / 4 + 1];
            for (r = h - 1; r > l && !buf[r]; r--) {
            }
            r++;
            out << format(PCL_POS_Y) % (tile->offsetY() + lconf->basey + offy + y);
            out << format(PCL_POS_X) % (tile->offsetX() + lconf->basex + offx + l);

            if (dir) {
              LOG_DEBUG_STR("reverse");
              out << format(R_INTENSITY) % (-(r - l));
              // reverse bytes!
              for (n = 0; n < (r - l) / 2; n++) {
                char t = buf[l + n];
                buf[l + n] = buf[r - n - 1];
                buf[r - n - 1] = t;
              }
            } else {
              out << format(R_INTENSITY) % (r - l);
            }
            dir = 1 - dir;
            // pack
            n = 0;
            while (l < r) {
              int p;
              for (p = l; p < r && p < l + 128 && buf[p] == buf[l]; p++) {
                ;
              }
              if (p - l >= 2) {
                // run length
                pack[n++] = 257 - (p - l);
                pack[n++] = buf[l];
                l = p;
              } else {
                for (p = l; p < r && p < l + 127 && (p + 1 == r || buf[p]
                    != buf[p + 1]); p++) {
                  ;
                }

                pack[n++] = p - l - 1;
                while (l < p) {
                  pack[n++] = buf[l++];
                }
              }
            }
            out << format(R_ROW_BYTES) % ((n + 7) / 8 * 8);
            r = 0;
            while (r < n) {
              out << pack[r++];
            }
            while (r & 7) {
              r++;
              out << "\x80";
            }
          }
        }
      }
    }

  }
}

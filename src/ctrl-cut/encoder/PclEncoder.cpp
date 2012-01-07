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


#include "util/PJL.hpp"
#include "util/2D.hpp"
#include "engrave/image/MMapMatrix.hpp"
#include "config/EngraveSettings.hpp"
#include "PclEncoder.hpp"

#include <boost/format.hpp>
#include <math.h>

using boost::format;

typedef EngraveSettings ES;

void PclEncoder::encode(std::ostream &out, Engraving& raster)
{
  return;
  // REFACTOR
  BitmapImage* image = &raster.front();
  if(!image) {
    LOG_WARN_STR("Engraving is not processed. Can't encode");
    return;
  }
  const Point& pos = raster.settings.get(EngraveSettings::EPOS);

  LOG_DEBUG_STR("Encode raster");
  ES::Direction direction = raster.settings.get(ES::DIRECTION);
  // Raster direction (1 = up)
  out << format(R_DIRECTION) % direction;
  // start at current position
  out << R_START;

  LOG_DEBUG_STR("Encoding bitmap image..");

  int width = image->width() / 8; // width in bytes
  int height = image->height();

  // raster (basic)
  uint32_t lasty = 0;
  bool dir = false;

  for (int i=0;i<height;i++) {
    uint32_t y;
    if (direction == ES::BOTTOMUP) y = height - i - 1;
    else y = i;

    uint8_t *scanline = (uint8_t *)image->data() + y*image->rowstride(); // Pointer to a scanline

    // find left/right of data (dir==0 ? left : right)
    int l;
    for (l = 0; l < width && (scanline[l] == 0xff); l++) { }

    if (l < width) {
      // a line to print
      int packsize = width * 2; // To be on the safe side
      char pack[packsize];
      // find left/right of data (dir==0 ? right : left )
      int r;
      for (r = width - 1; r > l && (scanline[r] == 0xff); r--) { }
      r++;

      uint32_t ypos = pos.y + y;

      // Epilog somehow outputs and END-DIR-START sequence for each 388 scanlines.
      // We'll do the same for now.
      // FIXME: Is this dependent on the DPI setting?
      // FIXME: If we're rendering multiple tiles, we need to keep track of the 
      // position of the previous one.
      if (ypos / 388 != lasty / 388) {
        out << R_END << format(R_DIRECTION) % direction << R_START;
      }
      lasty = ypos;

      out << format(PCL_POS_Y) % ypos;
      out << format(PCL_POS_X) % (pos.x + (l * 8));

      int n;
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
        int nextrun = r;
        // find next run of at least 3 equal bytes
        for (p = l; p < (r - 2); p++) {
          if (scanline[p] == scanline[p+1] && scanline[p] == scanline[p+2]) {
            nextrun = p;
            break;
          }
        }
        if (nextrun > l) { // copy until next run length
          p = l;
          while (nextrun - p > 0) {
            // copy
            int numbytes = nextrun - p;
            if (numbytes > 127) numbytes = 127;
            pack[n++] = numbytes - 1;
            for (int i=0;i<numbytes;i++) {
              pack[n++] = ~(scanline[p++]);
            }
          }
          l = p;
        }
        if (nextrun < r) {
          for (;p < (r - 1) && (scanline[p] == scanline[p+1]); p++) {}

          while (p - nextrun >= 0) {
            // fill
            int numbytes = p - nextrun + 1;
            if (numbytes > 128) numbytes = 128;
            pack[n++] = 257 - numbytes;
            pack[n++] = ~(scanline[p]);
            nextrun += numbytes;
          }
          l = p + 1;
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

  out << R_END; // end raster
}



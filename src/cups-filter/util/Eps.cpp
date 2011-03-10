/*
 * Ctrl-Cut - A laser cutter CUPS driver
 * Copyright (C) 2002-2008 Andrews & Arnold Ltd <info@aaisp.net.uk>
 * Copyright (C) 2008 AS220 Labs <brandon@as220.org>
 * Copyright (C) 2009-2010 Amir Hassan <amir@viel-zu.org> and Marius Kintel <marius@kintel.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <    >.
 *========================================================================
 *
 */
#include "Eps.h"


#include <stdio.h>
#include <stdlib.h>
#include <strings.h>

/*!
  This hack searches for a bbox at the end of the file, but in a really dumb way.
  Returns true if a bbox was found.
*/
bool get_bbox_from_end(cups_file_t *ps_file, 
                       int &lower_left_x, int &lower_left_y, 
                       int &upper_right_x, int &upper_right_y)
{
  while (cupsFileGets(ps_file, buf, sizeof(buf))) {
    if (!strncasecmp(buf, "%%BoundingBox:", 14)) {
      if (sscanf(buf + 14, "%d %d %d %d",
                 &lower_left_x, &lower_left_y, &upper_right_x, &upper_right_y) == 4) {
        return true;
      }
    }
  }
  return false;
}

bool get_bbox_from_string(const char *str, int &lower_left_x, int &lower_left_y, 
                          int &upper_right_x, int &upper_right_y)
{
  return (sscanf(str, "%d %d %d %d",
                 &lower_left_x, &lower_left_y, 
                 &upper_right_x, &upper_right_y) == 4);
}

/**
 * Convert the given postscript file (ps) converting it to an encapsulated
 * postscript file (eps).
 *
 * @param ps_file a file handle pointing to an opened postscript file that
 * is to be converted.
 * @param eps_file a file handle pointing to the opened encapsulated
 * postscript file to store the result.
 *
 * @return Return true if the function completes its task, false otherwise.
 */
bool ps_to_eps(LaserConfig *lconf, cups_file_t *ps_file, FILE *eps_file)
{
  int xoffset = 0;
  int yoffset = 0;
  bool created_by_cairo = false;
  bool bboxfound = false;
  bool landscape = false;
  bool portrait = false;

  int l;
  while (cupsFileGetLine(ps_file, (char *) buf, sizeof(buf))) {
    fprintf(eps_file, "%s", (char *) buf);
    if (*buf != '%') {
      continue; // We're only looking for comment lines
    }
    if (!strncasecmp((char *) buf, "%%Creator: cairo", 16)) {
      created_by_cairo = true;
    }
    else if (!strncasecmp((char *) buf, "%%EndPageSetup", 14)) {
      LOG_DEBUG_STR("%%EndPageSetup");
      // Check for Inkscape with landscape bug
      if (created_by_cairo && landscape && portrait) {
        LOG_DEBUG_STR("Broken Inkscape detected");
        // Revert Inkscape fuckup
        fprintf(eps_file, "0 -1 1 0 0 1728 6 array astore concat\n");
      }
    }
    else if (!strncasecmp((char *) buf, "%%PageOrientation: Landscape", 27)) {
      LOG_DEBUG_STR("%%PageOrientation: Landscape");
      landscape = true;
    }
    else if (!bboxfound && !strncasecmp((char *) buf, "%%PageBoundingBox:", 18)) {
      int lower_left_x;
      int lower_left_y;
      int upper_right_x;
      int upper_right_y;
      bboxfound = get_bbox_from_string(buf + 18, lower_left_x, lower_left_y, upper_right_x, upper_right_y);
      if (bboxfound) {
        LOG_DEBUG_STR("Found bbox from %%PageBoundingBox");
        xoffset = lower_left_x;
        yoffset = lower_left_y;
        int width = upper_right_x - lower_left_x;
        int height = upper_right_y - lower_left_y;

        // FIXME: This is a check for buggy postscript from inkscape.
        // Inkscape 0.46 works, 0.48 is broken. The broken one swaps width and height.
        // If the laser itself has a portrait layout, this check is inherently wrong
        // -> rethink this later.
        if (height > width) {
          portrait = true;
        }

        // FIXME: Sometimes (e.g. from Inkscape) the width and height is swapped.
        // We don't want ot poison our lconf with this, so comment this out for now.
        // lconf->width = width;
        // lconf->height = height;

        // FIXME: Commented out for now as I don't know what this was supposed to do.
        fprintf(eps_file, "0 %d neg translate\n", lconf->height - upper_right_y);

#if 0
        fprintf(eps_file, "/setpagedevice{pop}def\n"); // use bbox
        fprintf(eps_file, "0 %d translate\n", lconf->height - upper_right_y);
        if (xoffset || yoffset) {
          fprintf(eps_file, "%d %d translate\n", -xoffset, -yoffset);
        }
        if (lconf->flip) {
          fprintf(eps_file, "%d 0 translate -1 1 scale\n",
                  lconf->width);
        }
#endif
      }
    }
    else if (!bboxfound && !strncasecmp((char *) buf, "%%BoundingBox:", 14)) {
      int lower_left_x;
      int lower_left_y;
      int upper_right_x;
      int upper_right_y;
      bboxfound = get_bbox_from_string(buf + 14, lower_left_x, lower_left_y, upper_right_x, upper_right_y);
      if (bboxfound) {
        LOG_DEBUG_STR("Found bbox from %%BoundingBox");
        xoffset = lower_left_x;
        yoffset = lower_left_y;
        // int width = upper_right_x - lower_left_x;
        // int height = upper_right_y - lower_left_y;
        fprintf(eps_file, "0 %d translate\n", lconf->height - upper_right_y);
      }
    }
    else if (!strncasecmp((char *) buf, "%!", 2)) { // Start of document
      // Define === to print whatever is on the stack
      fprintf(eps_file, "/=== { (        ) cvs print } def\n");
      // Redefine stroke to also print the coordinates
      fprintf(eps_file,
              "/stroke { " // define stroke
              "currentlinewidth " // Put current line width on stack
              "matrix currentmatrix " // Get current matrix
              "0 get mul 5 lt " // Check linewidth (hackish; only checks the matrix x axis scale)
              // If linewidth < 5, this will be cut:
              // Print "P" followed by a number for the amount of red (0-100)
              "{(P)=== currentrgbcolor pop pop 100 mul round cvi = "
              "flattenpath " // Convert all graphics to lines
              "{transform (M)=== 1 sub round cvi === (,)=== 1 sub round cvi =}" // move
              "{transform (L)=== 1 sub round cvi === (,)=== 1 sub round cvi =}" // line
              "{}" // curve (shouldn't occur due to flattenpath)
              "{(C)=}" // close
              "pathforall " // execute one of the previous 4 blocks
              "newpath}" // remove path
              // If linewidth >= 5, this will be lasercut:
              "{stroke}"
              "ifelse "              // The actual ifelse statement - reverse boolean ftw!
              "} bind def\n"); // end define stroke
      // Redefine showpage to first print "X"
      fprintf(eps_file, "/showpage { (X)= showpage } bind def\n");

      // FIXME: Go through and verify this code
      if (lconf->raster_mode != 'c' && lconf->raster_mode != 'g') {
        if (lconf->screen == 0) {
          fprintf(eps_file, "{0.5 ge{1}{0}ifelse}settransfer\n");
        } else {
          int s = lconf->screen;
          if (s < 0) {
            s = 0 - s;
          }
          if (lconf->resolution >= 600) {
            // adjust for overprint
            fprintf(eps_file,
                    "{dup 0 ne{%d %d div add}if}settransfer\n",
                    lconf->resolution / 600, s);
          }
          fprintf(eps_file, "%d 30{%s}setscreen\n", lconf->resolution
                  / s, (lconf->screen > 0) ? "pop abs 1 exch sub"
                  : "180 mul cos exch 180 mul cos add 2 div");
        }
      }
    }
  }
  while ((l = cupsFileRead(ps_file, (char *) buf, sizeof(buf))) > 0) {
    fwrite((char *) buf, 1, l, eps_file);
  }
  return true;
}

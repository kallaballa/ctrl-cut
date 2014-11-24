/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "Eps.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <locale>
#include <sstream>

/*!
  This hack searches for a bbox at the end of the file, but in a really dumb way.
  Returns true if a bbox was found.
*/
bool get_bbox_from_end(FILE *ps_file,
                       int &lower_left_x, int &lower_left_y, 
                       int &upper_right_x, int &upper_right_y)
{
  char buf[102400];
  while (fgets(buf, sizeof(buf), ps_file)) {
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
bool ps_to_eps(FILE *ps_file, FILE *eps_file, uint32_t resolution)
{
  bool created_by_cairo = false;
  bool bboxfound = false;
  bool landscape = false;
  bool portrait = false;
  bool startfound = false;
  char buf[102400];
  int l;
  unsigned char c;

  // skip everything at the start of the file until you hit ps code
  while ((c = fgetc(ps_file)) != EOF && c != '%')
  {}

  fprintf(eps_file, "%c", '%');

  while (fgets(buf, sizeof(buf), ps_file)) {
    size_t bufLen = strlen(buf);
    //convert windows line ending to unix line ending
    if(bufLen > 1 && !strncmp(buf + bufLen - 2, "\r\n", 2)) {
      buf[bufLen - 2] = '\n';
      buf[bufLen - 1] = '\0';
    }

    fprintf(eps_file, "%s", (char *) buf);

    //immediatly stop at EOF because some programs inject thumbnails after the document
    if (!strncasecmp((char *) buf, "%%EOF", 5)) {
      return true;
    }

    if (*buf != '%' && *buf != '!') {
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

        // FIXME: Commented out as it triggers a bug when printing from Inkscape. kintel 20110317
        // fprintf(eps_file, "0 %d neg translate\n", lconf->height - upper_right_y);

        //        fprintf(eps_file, "0 %d translate\n", lconf->height - upper_right_y);

        // FIXME: Commented out for now as I don't know what this was supposed to do.
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
        // int width = upper_right_x - lower_left_x;
        // int height = upper_right_y - lower_left_y;

        // FIXME: Commented out as it triggers a bug when printing from Inkscape. kintel 20110317
        //        fprintf(eps_file, "0 %d translate\n", lconf->height - upper_right_y);
      }
    }
    else if (!startfound && (!strncasecmp((char *) buf, "%!PS", 4) || !strncasecmp((char *) buf, "!PS", 3))) { // Start of document
      double linethreshold = 1.9 - (1.0 - (resolution / 600.0));
      if(resolution == 1200)
        linethreshold += 5;

      std::stringstream ss;
      ss.imbue(std::locale("C"));
      ss << linethreshold;

      startfound = true;
      // Define === to print whatever is on the stack
      fprintf(eps_file, "/=== { (        ) cvs print } def\n");
      // Redefine stroke to also print the coordinates
      fprintf(eps_file,
              "/OriginalStroke /stroke load def " //save original stroke
              "/stroke { " // redefine stroke
              "OriginalStroke " //draw stroke
              "(R)=== mark currentcolor counttomark -1 1 { -1 roll 20 string cvs print (,) print} for flush pop (\n) === "
              "currentlinewidth " // Put current line width on stack
              "matrix currentmatrix " // Get current matrix
              "0 get mul %s lt " // Check linewidth (hackish; only checks the matrix x axis scale)
              // If linewidth < 3 points, this will be cut.
              // FIXME: We should decide which threshold to use.
              // 5 points is ca. 1.75 mm which is way too much.
              // e.g. 0.025 mm would amount to 0.07 points.
              // Print "P" followed by a number for the amount of red (0-100)
              "{ "
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
              "} bind def\n", ss.str().c_str()); // end define stroke
      // Redefine showpage to first print "X"
      fprintf(eps_file, "/showpage { (X)= showpage } bind def\n");
    }
  }
  while ((l = fread((char *) buf, 1, sizeof(buf), ps_file)) > 0) {
    fwrite((char *) buf, 1, l, eps_file);
  }
  return true;
}

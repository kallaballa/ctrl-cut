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
#include <iostream>
#include <sstream>
#include <string>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

using std::istringstream;
using std::string;

#define BITMAP_HEADER_NBYTES (54)

/** Default on whether or not auto-focus is enabled. */
#define AUTO_FOCUS (1)

/** Default bed height (y-axis) in pts. */
#define BED_HEIGHT (1728)

/** Default bed width (x-axis) in pts. */
#define BED_WIDTH (2592)

/**
 * Default on whether or not the result is supposed to be flipped along the X
 * axis.
 */
#define FLIP (0)

/** Additional offset for the X axis. */
#define HPGLX (0)

/** Additional offset for the Y axis. */
#define HPGLY (0)

/** Accepted number of points per an inch. */
#define POINTS_PER_INCH (72)

/** Default mode for processing raster engraving (varying power depending upon
 * image characteristics).
 * Possible values are:
 * 'c' = color determines power level
 * 'g' = grey-scale levels determine power level
 * 'm' = mono mode
 * 'n' = no rasterization
 */
#define RASTER_MODE_DEFAULT 'g'

/** Default power level for raster engraving */
#define RASTER_POWER_DEFAULT (20)

/** Whether or not the raster printing is to be repeated. */
#define RASTER_REPEAT (1)

/** Default speed for raster engraving */
#define RASTER_SPEED_DEFAULT (100)

/** Default resolution is 600 DPI */
#define RESOLUTION_DEFAULT (600)

/** Pixel size of screen (0 is threshold).
 * FIXME - add more details
 */
#define SCREEN_DEFAULT (8)

/** FIXME */
#define VECTOR_FREQUENCY_DEFAULT (5000)

/** Default power level for vector cutting. */
#define VECTOR_POWER_DEFAULT (10)

/** Default speed level for vector cutting. */
#define VECTOR_SPEED_DEFAULT (100)

typedef struct {
  /** Variable to track auto-focus. */
  int focus;

  /** Height of the image (y-axis). By default this is the bed's height. */
  int height;

  /** Variable to track the resolution of the print. */
  int resolution;

  /** Variable to track the mode for rasterization. One of color 'c', or
   * grey-scale 'g', mono 'm', or none 'n'
   */
  char raster_mode;

  /** Variable to track the raster speed. */
  int raster_speed;

  /** Variable to track the raster power. */
  int raster_power;

  /** Variable to track whether or not a rasterization should be repeated. */
  int raster_repeat;

  /** FIXME -- pixel size of screen, 0= threshold */
  int screen;

  /** Variable to track the vector speed. */
  int vector_speed;

  /** Variable to track the vector power. */
  int vector_power;

  /** Variable to track the vector frequency. FIXME */
  int vector_freq;

  /** Width of the image (x-axis). By default this is the bed's width. */
  int width;

  /** X re-center (0 = not). */
  int x_center;

  /** Track whether or not to repeat X. */
  int x_repeat;

  /** Y re-center (0 = not). */
  int y_center;

  /** Track whether or not to repeat X. */
  int y_repeat;

  /** The relative x position inside the laser bed to start operating from **/
  int basex;

  /** The relative y position inside the laser bed to start operating from **/
  int basey;

  /** Variable to track whether or not the X axis should be flipped. */
  char flip;

} laser_config;

void calculate_base_position(laser_config *lconf)
{
  if (lconf->x_center) {
      lconf->basex = lconf->x_center - lconf->width / 2;
    }
    if (lconf->y_center) {
      lconf->basey = lconf->y_center - lconf->height / 2;
    }
    if (lconf->basex < 0) {
      lconf->basex = 0;
    }
    if (lconf->basey < 0) {
      lconf->basey = 0;
    }
    // rasterises
    lconf->basex = lconf->basex * lconf->resolution / POINTS_PER_INCH;
    lconf->basey = lconf->basey * lconf->resolution / POINTS_PER_INCH;
}

void init_laser_config(laser_config *lconf)
{
  lconf->focus = AUTO_FOCUS;
  lconf->height = BED_HEIGHT;
  lconf->resolution = RESOLUTION_DEFAULT;
  lconf->raster_mode = RASTER_MODE_DEFAULT;
  lconf->raster_speed = RASTER_SPEED_DEFAULT;
  lconf->raster_power = RASTER_POWER_DEFAULT;
  lconf->raster_repeat = RASTER_REPEAT;
  lconf->screen = SCREEN_DEFAULT;
  lconf->vector_speed = VECTOR_SPEED_DEFAULT;
  lconf->vector_power = VECTOR_POWER_DEFAULT;
  lconf->vector_freq = VECTOR_FREQUENCY_DEFAULT;
  lconf->width = BED_WIDTH;
  lconf->x_repeat = 1;
  lconf->y_repeat = 1;
  lconf->basex = 0;
  lconf->basey = 0;
  lconf->flip = FLIP;
  calculate_base_position(lconf);
}

const char* SEP = ";";

/**
 * PJL COMMANDS
 * **/

/* Print the printer job language header. */
const char* PJL_HEADER =
    "\e%%-12345X@PJL JOB NAME=%s\r\n\eE@PJL ENTER LANGUAGE=PCL\r\n";

/* End job. */
const char* PJL_FOOTER = "@PJL EOJ \r\n";


/**
 * PCL COMMANDS
 * **/

/* Set autofocus on or off. */
const char* PCL_AUTOFOCUS = "\e&y%dA";

/* Left (long-edge) offset registration.  Adjusts the position of the
 * logical page across the width of the page.
 */
const char* PCL_OFF_X = "\e&l%dU";

/* Top (short-edge) offset registration.  Adjusts the position of the
 * logical page across the length of the page.
 */
const char* PCL_OFF_Y = "\e&l%dZ";

/* Resolution of the print. */
const char* PCL_PRINT_RESOLUTION = "\e&u%dD";

/* PCL resolution. */
const char* PCL_RESOLUTION = "\e*t%dR";

/* FIXME: unknown purpose. */
const char* PCL_UNKNOWN_BLAFOO =  "\e&y0C";

/* FIXME: unknown purpose. */
const char* PCL_UNKNOWN_BLAFOO2 =  "\e&y0Z";

/* FIXME: unknown purpose. */
const char* PCL_UNKNOWN_BLAFOO3 =  "\e&z2A";

/* Postition cursor absolute on the X-axis */
const char* PCL_POS_X = "\e*p%dX";

/* Postition cursor absolute on the Y-axis */
const char* PCL_POS_Y = "\e*p%dY";

/* PCL section end */
const char* PCL_SECTION_END = "\e%%1B";

/* Reset PCL */
const char* PCL_RESET = "\eE";

/* Exit PCL */
const char* PCL_EXIT = "\e%%-12345X";


/**
 * PCL RASTER COMMANDS
 * **/

/* Raster Orientation */
const char* R_ORIENTATION = "\e*r%dF";

/* Raster power */
const char* R_POWER = "\e&y%dP";

/* Raster speed */
const char* R_SPEED = "\e&z%dS";

/* Raster height */
const char* R_HEIGHT = "\e*r%dT";

/* Raster width */
const char* R_WIDTH = "\e*r%dS";

/* Raster compression */
const char* R_COMPRESSION = "\e*b%dM";

/* Raster direction (1 = up) */
const char* R_DIRECTION_UP = "\e&y1O";

/* Start raster operation at current position */
const char* R_START_AT_POS = "\e*r1A";

/* The intensity (relative amount of the defined R_POWER) used for the current raster row */
const char* R_INTENSITY = "\e*b%dA";

/* The number of bytes in the raster row */
const char* R_ROW_BYTES = "\e*b%dW";

/* Guessed: resets raster job */
const char* R_RESET = "\e*rC";


/**
 * PCL VECTOR COMMANDS
 * **/

/* Intialize vector mode */
const char* V_INIT = "IN";

/* Set laser pulse frequency */
const char* V_FREQUENCY = "XR%04d";

/* Set laser power */
const char* V_POWER = "YP%03d";

/* Set laser speed */
const char* V_SPEED = "ZS%03d";


/**
 * HPGL COMMANDS
 * **/

/**
 * Pen up. This command doesnt comform to PCL/HPGL and is only used on the first issue of PU
 * There seems to be no difference to using the standard command also for the first issue, so
 * i use it only because the windows driver does.
 * */
const char* HPGL_PEN_UP_INIT = "LTPU";

/* Pen up. */
const char* HPGL_PEN_UP = "PU";

/* Pen up. */
const char* HPGL_PEN_DOWN = "PD";

/* HPGL section end */
const char* HPGL_END = "\e%%0B";


int stringtoint(string s) {
  istringstream iss(s);
  int i;
  iss >> i;
  return i;
}

void printUsage() {
  std::cerr << "Usage s:p:r:w:h:o:" << std::endl;
}

int big_to_little_endian(uint8_t *position, int nbytes)
{
    int i;
    int result = 0;

    for (i = 0; i < nbytes; i++) {
        result += *(position + i) << (8 * i);
    }
    return result;
}

bool generate_raster(laser_config *lconf, FILE *bitmap_file, FILE *pcl_file) {
  int i;

    /* Print the printer job language header. */
    fprintf(pcl_file, PJL_HEADER, "bmp2pcl");
    /* Set autofocus on or off. */
    fprintf(pcl_file, PCL_AUTOFOCUS, lconf->focus);
    /* FIXME unknown purpose. */
    fprintf(pcl_file, PCL_UNKNOWN_BLAFOO);
    /* FIXME unknown purpose. */
    fprintf(pcl_file, PCL_UNKNOWN_BLAFOO2);
    /* Left (long-edge) offset registration.  Adjusts the position of the
     * logical page across the width of the page.
     */
    fprintf(pcl_file, PCL_OFF_X, 0);
    /* Top (short-edge) offset registration.  Adjusts the position of the
     * logical page across the length of the page.
     */
    fprintf(pcl_file, PCL_OFF_Y, 0);
    /* Resolution of the print. */
    fprintf(pcl_file, PCL_PRINT_RESOLUTION, lconf->resolution);
    /* X position = 0 */
    fprintf(pcl_file, PCL_POS_X, 0);
    /* Y position = 0 */
    fprintf(pcl_file, PCL_POS_Y, 0);
    /* PCL resolution. */
    fprintf(pcl_file, PCL_RESOLUTION, lconf->resolution);

  int h;
  int d;
  int offx;
  int offy;
  int repeat;
  char buf[102400];
  uint8_t bitmap_header[BITMAP_HEADER_NBYTES];

  repeat = lconf->raster_repeat;
  while (repeat--) {
    /* repeated (over printed) */
    int pass;
    int passes;
    long base_offset;
    if (lconf->raster_mode == 'c') {
      passes = 7;
    } else {
      passes = 1;
    }

    /* Read in the bitmap header. */
    fread(bitmap_header, 1, BITMAP_HEADER_NBYTES, bitmap_file);

    /* Re-load width/height from bmp as it is possible that someone used
     * setpagedevice or some such
     */
    /* Bytes 18 - 21 are the bitmap width (little endian format). */
    lconf->width = big_to_little_endian(bitmap_header + 18, 4);

    /* Bytes 22 - 25 are the bitmap height (little endian format). */
    lconf->height = big_to_little_endian(bitmap_header + 22, 4);

    /* Bytes 10 - 13 base offset for the beginning of the bitmap data. */
    base_offset = big_to_little_endian(bitmap_header + 10, 4);

    if (lconf->raster_mode == 'c' || lconf->raster_mode == 'g') {
      /* colour/grey are byte per pixel power levels */
      h = lconf->width;
      /* BMP padded to 4 bytes per scan line */
      d = (h * 3 + 3) / 4 * 4;
    } else {
      /* mono */
      h = (lconf->width + 7) / 8;
      /* BMP padded to 4 bytes per scan line */
      d = (h + 3) / 4 * 4;
    }

    /* Raster Orientation */
    fprintf(pcl_file, R_ORIENTATION, 0);
    /* Raster power */
    fprintf(pcl_file, R_POWER, (lconf->raster_mode == 'c' || lconf->raster_mode
        == 'g') ? 100 : lconf->raster_power);
    /* Raster speed */
    fprintf(pcl_file, PCL_UNKNOWN_BLAFOO3);
    fprintf(pcl_file, R_SPEED, lconf->raster_speed);
    fprintf(pcl_file, R_HEIGHT, lconf->height * lconf->y_repeat);
    fprintf(pcl_file, R_WIDTH, lconf->width * lconf->x_repeat);
    /* Raster compression */
    fprintf(pcl_file, R_COMPRESSION, (lconf->raster_mode == 'c'
        || lconf->raster_mode == 'g') ? 7 : 2);
    /* Raster direction (1 = up) */
    fprintf(pcl_file, R_DIRECTION_UP);
    /* start at current position */
    fprintf(pcl_file, R_START_AT_POS);
    for (offx = lconf->width * (lconf->x_repeat - 1); offx >= 0; offx
        -= lconf->width) {
      for (offy = lconf->height * (lconf->y_repeat - 1); offy >= 0; offy
          -= lconf->height) {
        for (pass = 0; pass < passes; pass++) {
          // raster (basic)
          int y;
          char dir = 0;

          fseek(bitmap_file, base_offset, SEEK_SET);
          for (y = lconf->height - 1; y >= 0; y--) {
            int l;

            switch (lconf->raster_mode) {
            case 'c': // colour (passes)
            {
              char *f = buf;
              char *t = buf;
              if (d > sizeof(buf)) {
                perror("Too wide");
                return false;
              }
              l = fread((char *) buf, 1, d, bitmap_file);
              if (l != d) {
                fprintf(stderr, "Bad bit data from gs %d/%d (y=%d)\n", l, d, y);
                return false;
              }
              while (l--) {
                // pack and pass check RGB
                int n = 0;
                int v = 0;
                int p = 0;
                int c = 0;
                for (c = 0; c < 3; c++) {
                  if (*f > 240) {
                    p |= (1 << c);
                  } else {
                    n++;
                    v += *f;
                  }
                  f++;
                }
                if (n) {
                  v /= n;
                } else {
                  p = 0;
                  v = 255;
                }
                if (p != pass) {
                  v = 255;
                }
                *t++ = 255 - v;
              }
            }
              break;
            case 'g': // grey level
            {
              /* BMP padded to 4 bytes per scan line */
              int d = (h + 3) / 4 * 4;
              if (d > sizeof(buf)) {
                fprintf(stderr, "Too wide\n");
                return false;
              }
              l = fread((char *) buf, 1, d, bitmap_file);
              if (l != d) {
                fprintf(stderr, "Bad bit data from gs %d/%d (y=%d)\n", l, d, y);
                return false;
              }
              for (l = 0; l < h; l++) {
                buf[l] = (255 - (uint8_t) buf[l]);
              }
            }
              break;
            default: // mono
            {
              int d = (h + 3) / 4 * 4; // BMP padded to 4 bytes per scan line
              if (d > sizeof(buf)) {
                perror("Too wide");
                return false;
              }
              l = fread((char *) buf, 1, d, bitmap_file);
              if (l != d) {
                fprintf(stderr, "Bad bit data from gs %d/%d (y=%d)\n", l, d, y);
                return false;
              }
            }
            }

            if (lconf->raster_mode == 'c' || lconf->raster_mode == 'g') {
              for (l = 0; l < h; l++) {
                /* Raster value is multiplied by the
                 * power scale.
                 */
                buf[l] = (uint8_t) buf[l] * lconf->raster_power / 255;
              }
            }

            /* find left/right of data */
            for (l = 0; l < h && !buf[l]; l++) {
              ;
            }

            if (l < h) {
              /* a line to print */
              int r;
              int n;
              unsigned char pack[sizeof(buf) * 5 / 4 + 1];
              for (r = h - 1; r > l && !buf[r]; r--) {
                ;
              }
              r++;
              fprintf(pcl_file, PCL_POS_Y, lconf->basey + offy + y);
              fprintf(pcl_file, PCL_POS_X,
                  lconf->basex + offx + ((lconf->raster_mode == 'c'
                      || lconf->raster_mode == 'g') ? l : l * 8));
              if (dir) {
                fprintf(pcl_file, R_INTENSITY, -(r - l));
                // reverse bytes!
                for (n = 0; n < (r - l) / 2; n++) {
                  unsigned char t = buf[l + n];
                  buf[l + n] = buf[r - n - 1];
                  buf[r - n - 1] = t;
                }
              } else {
                fprintf(pcl_file, R_INTENSITY, (r - l));
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
              fprintf(pcl_file, R_ROW_BYTES, (n + 7) / 8 * 8);
              r = 0;
              while (r < n)
                fputc(pack[r++], pcl_file);
              while (r & 7) {
                r++;
                fputc(0x80, pcl_file);
              }
            }
          }
        }
      }
    }
    fprintf(pcl_file, "\e*rC"); // end raster
    fputc(26, pcl_file); // some end of file markers
    fputc(4, pcl_file);
  }

  fprintf(pcl_file, PCL_RESET);
  fprintf(pcl_file, PCL_EXIT);
  fprintf(pcl_file, PJL_FOOTER);

  /* Pad out the remainder of the file with 0 characters. */
  for(i = 0; i < 4096; i++) {
      fputc(0, pcl_file);
  }

  return true;
}
int main(int argc, char **argv) {
  int resolution = 600;
  int width = 2592;
  int height = 1728;
  int speed = 100;
  int power = 10;

  string* ifilename = NULL;
  string* ofilename = NULL;

  int c;

  opterr = 0;

  while ((c = getopt(argc, argv, "s:p:r:w:h:o:")) != -1)
    switch (c) {
    case 's':
      speed = stringtoint(optarg);
      break;
    case 'p':
      power = stringtoint(optarg);
      break;
    case 'r':
      resolution = stringtoint(optarg);
      break;
    case 'w':
      width = stringtoint(optarg);
      break;
    case 'h':
      height = stringtoint(optarg);
      break;
    case 'o':
      ofilename = new string(optarg);
      break;
    case '?':
      if (optopt == 'c')
        fprintf(stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      return 1;
    default:
      abort();
    }

  if (optind < argc) {
    ifilename = new string(argv[optind]);
  } else {
    printUsage();
    exit(1);
  }

  if (optind + 1 < argc) {
    ofilename = new string(argv[optind + 1]);
  } else {
    ofilename = new string("/dev/stdout");
    exit(1);
  }

  laser_config lconf;
  init_laser_config(&lconf);
  lconf.width = width;
  lconf.height = height;
  lconf.resolution = resolution;
  lconf.raster_speed = speed;
  lconf.raster_power = power;
  FILE* bmp = fopen(ifilename->c_str(), "r");
  FILE* pcl = fopen(ofilename->c_str(), "w");
  generate_raster(&lconf, bmp, pcl);


  return 0;
}


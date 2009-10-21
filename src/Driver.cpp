/*
 * Driver.cpp
 *
 *  Created on: 18.10.2009
 *      Author: amir
 */

#include "Driver.h"
#include "CuttingOptimizer.h"
#include <vector>
#include "LaserJob.h"
#include "pjl.h"
#include "CupsParser.h"

Driver::Driver() {
	// TODO Auto-generated constructor stub

}

Driver::~Driver() {
	// TODO Auto-generated destructor stub
}

void Driver::process(LaserJob *job) {
	CuttingOptimizer co;
	list<LaserPass*> passes = job->getPasses();

	list<LaserPass*>::iterator it;
	VectorPass *vpass;
	for (it = passes.begin(); it != passes.end(); it++) {
		vpass = dynamic_cast<VectorPass*> (*it);
		if (vpass) {
			co.optimizeVectorPass(vpass);
		}
	}
}


/**
 * Convert a big endian value stored in the array starting at the given pointer
 * position to its little endian value.
 *
 * @param position the starting location for the conversion. Each successive
 * unsigned byte is upto nbytes is considered part of the value.
 * @param nbytes the number of successive bytes to convert.
 *
 * @return An integer containing the little endian value of the successive
 * bytes.
 */
int big_to_little_endian(uint8_t *position, int nbytes) {
	int i;
	int result = 0;

	for (i = 0; i < nbytes; i++) {
		result += *(position + i) << (8 * i);
	}
	return result;
}
/*
bool generate_raster(RasterPass *rpass, FILE *bitmap_file, FILE *pjl_file) {
	int h;
	int d;
	int offx;
	int offy;
	int repeat;
	uint8_t bitmap_header[BITMAP_HEADER_NBYTES];

	repeat = rpass->lconf->raster_repeat;
	while (repeat--) {
		// repeated (over printed)
		int pass;
		int passes;
		long base_offset;
		if (lconf->raster_mode == 'c') {
			passes = 7;
		} else {
			passes = 1;
		}

		// Read in the bitmap header.
		fread(bitmap_header, 1, BITMAP_HEADER_NBYTES, bitmap_file);

		// Re-load width/height from bmp as it is possible that someone used  setpagedevice or some such
		// Bytes 18 - 21 are the bitmap width (little endian format).
		lconf->width = big_to_little_endian(bitmap_header + 18, 4);

		// Bytes 22 - 25 are the bitmap height (little endian format).
		lconf->height = big_to_little_endian(bitmap_header + 22, 4);

		// Bytes 10 - 13 base offset for the beginning of the bitmap data.
		base_offset = big_to_little_endian(bitmap_header + 10, 4);
char buf[102400];
		if (lconf->raster_mode == 'c' || lconf->raster_mode == 'g') {
			// colour/grey are byte per pixel power levels
			h = lconf->width;
			// BMP padded to 4 bytes per scan line
			d = (h * 3 + 3) / 4 * 4;
		} else {
			// mono
			h = (lconf->width + 7) / 8;
			// BMP padded to 4 bytes per scan line
			d = (h + 3) / 4 * 4;
		}
		if (debug) {
			fprintf(stderr, "Width %d Height %d Bytes %d Line %d\n",
					lconf->width, lconf->height, h, d);
		}

		// Raster Orientation
		fprintf(pjl_file, R_ORIENTATION, 0);
		// Raster power
		fprintf(pjl_file, R_POWER, (lconf->raster_mode == 'c'
				|| lconf->raster_mode == 'g') ? 100 : lconf->raster_power);
		// Raster speed

		fprintf(pjl_file, PCL_UNKNOWN_BLAFOO3);
		fprintf(pjl_file, R_SPEED, lconf->raster_speed);
		fprintf(pjl_file, R_HEIGHT, lconf->height * lconf->y_repeat);
		fprintf(pjl_file, R_WIDTH, lconf->width * lconf->x_repeat);
		// Raster compression
		fprintf(pjl_file, R_COMPRESSION, (lconf->raster_mode == 'c'
				|| lconf->raster_mode == 'g') ? 7 : 2);
		// Raster direction (1 = up)
		fprintf(pjl_file, R_DIRECTION_UP);
		// start at current position
		fprintf(pjl_file, R_START_AT_POS);
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
							tion »bool generate_raster(RasterPass*, FILE*, FILE*)«:
							../src/Driver.cpp:77: Fehler: »lconf« wurde in diesem Gültigkeitsbereich nicht definiert
							../src/Driver.cpp:90: Fehler: »lconf« wurd			if (l != d) {
								fprintf(stderr,
										"Bad bit data from gs %d/%d (y=%d)\n",
										l, d, y);
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
							//BMP padded to 4 bytes per scan line
							int d = (h + 3) / 4 * 4;
							if (d > sizeof(buf)) {
								fprintf(stderr, "Too wide\n");
								return false;
							}
							l = fread((char *) buf, 1, d, bitmap_file);
							if (l != d) {
								fprintf(stderr,
										"Bad bit data from gs %d/%d (y=%d)\n",
										l, d, y);
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
								fprintf(stderr,
										"Bad bit data from gs %d/%d (y=%d)\n",
										l, d, y);
								return false;
							}
						}
						}

						if (lconf->raster_mode == 'c' || lconf->raster_mode
								== 'g') {
							for (l = 0; l < h; l++) {
								//Raster value is multiplied by the power scale.
								buf[l] = (uint8_t) buf[l] * lconf->raster_power
										/ 255;
							}
						}

						// find left/right of data
						for (l = 0; l < h && !buf[l]; l++) {
							;
						}

						if (l < h) {
							// a line to print
							int r;
							int n;
							unsigned char pack[sizeof(buf) * 5 / 4 + 1];
							for (r = h - 1; r > l && !buf[r]; r--) {
								;
							}
							r++;
							fprintf(pjl_file, PCL_POS_Y, lconf->basey + offy
									+ y);
							fprintf(pjl_file, PCL_POS_X, lconf->basex + offx
									+ ((lconf->raster_mode == 'c'
											|| lconf->raster_mode == 'g') ? l
											: l * 8));
							if (dir) {
								fprintf(pjl_file, R_INTENSITY, -(r - l));
								// reverse bytes!
								for (n = 0; n < (r - l) / 2; n++) {
									unsigned char t = buf[l + n];
									buf[l + n] = buf[r - n - 1];
									buf[r - n - 1] = t;
								}
							} else {
								fprintf(pjl_file, R_INTENSITY, (r - l));
							}
							dir = 1 - dir;
							// pack
							n = 0;
							while (l < r) {
								int p;
								for (p = l; p < r && p < l + 128 && buf[p]
										== buf[l]; p++) {
									;
								}
								if (p - l >= 2) {
									// run length
									pack[n++] = 257 - (p - l);
									pack[n++] = buf[l];
									l = p;
								} else {
									for (p = l; p < r && p < l + 127 && (p + 1
											== r || buf[p] != buf[p + 1]); p++) {
										;
									}

									pack[n++] = p - l - 1;
									while (l < p) {
										pack[n++] = buf[l++];
									}
								}
							}
							fprintf(pjl_file, R_ROW_BYTES, (n + 7) / 8 * 8);
							r = 0;
							while (r < n)
								fputc(pack[r++], pjl_file);
							while (r & 7) {
								r++;
								fputc(0x80, pjl_file);
							}
						}
					}
				}
			}
		}
		fprintf(pjl_file, "\e*rC"); // end raster
		fputc(26, pjl_file); // some end of file markers
		fputc(4, pjl_file);
	}
	return true;
}
*/

/**
 *
 */
bool generate_vector(VectorPass *vpass, FILE *pjl_file) {
	char first = 1;
	int power = 100;

	fprintf(pjl_file, V_INIT);
	fprintf(pjl_file, SEP);
	fprintf(pjl_file, V_FREQUENCY, vpass->lconf->vector_freq);
	fprintf(pjl_file, SEP);
	fprintf(pjl_file, V_POWER, vpass->lconf->vector_power);
	fprintf(pjl_file, SEP);
	fprintf(pjl_file, V_SPEED, vpass->lconf->vector_speed);
	fprintf(pjl_file, SEP);


	vector<OnionSkin*> skins = vpass->skins;
	vector<OnionSkin*>::iterator it;
	list<LineSegment*>::iterator it_s;
	OnionSkin* skin;
	LineSegment* ls;

	for (it = skins.begin(); it != skins.end(); it++) {
		skin = *it;

		for (it_s = skin->segments.begin(); it_s != skin->segments.end(); it_s++) {
			ls = *it_s;

            if(ls->getPower() != power) {
            	power = ls->getPower();
            	int epower;
				if(first){
					fprintf(pjl_file, SEP);
					fprintf(pjl_file, HPGL_PEN_UP_INIT);
				} else {
					fprintf(pjl_file, SEP);
					fprintf(pjl_file, HPGL_PEN_UP);
				}

				epower = (power * vpass->lconf->vector_power + 50) / 100;
				if (vpass->lconf->vector_speed && vpass->lconf->vector_speed < 100) {
					int espeed = vpass->lconf->vector_speed;
					int efreq = vpass->lconf->vector_freq;
					if (epower && power < 100) {
						int r;
						int q;
						r = 10000 / power; // power, up to set power level (i.e. x=100)
						q = 10000 / espeed;
						if (q < r)
							r = q;
						q = 500000 / efreq;
						if (q < r)
							r = q;
						epower = (50 + epower * r) / 100;
						espeed = (50 + espeed * r) / 100;
						efreq = (50 + espeed * r) / 100;
					}

					fprintf(pjl_file, SEP);
					fprintf(pjl_file, V_SPEED, espeed);
					fprintf(pjl_file, SEP);
					fprintf(pjl_file, V_FREQUENCY, efreq);
					fprintf(pjl_file, SEP);
				}
				fprintf(pjl_file, V_POWER, epower);
				fprintf(pjl_file, SEP);
            }

            if (first) {
				fprintf(pjl_file, HPGL_PEN_UP_INIT);
			} else {
				fprintf(pjl_file, HPGL_PEN_UP);
			}

			fprintf(pjl_file, "%d,%d", vpass->lconf->basex + ls->getStart()->getY() + HPGLX, vpass->lconf->basey + ls->getStart()->getY() + HPGLY);
			fprintf(pjl_file, SEP);
			fprintf(pjl_file, HPGL_PEN_DOWN);
			fprintf(pjl_file, "%d,%d", vpass->lconf->basex + ls->getEnd()->getX() + HPGLX, vpass->lconf->basey + ls->getEnd()->getY() + HPGLY);

			first = 0;
		}
	}

    fprintf(pjl_file, ";");
    fprintf(pjl_file, HPGL_END);      // end HLGL
    fprintf(pjl_file, PCL_SECTION_END);
    fprintf(pjl_file, HPGL_PEN_UP);

	return true;
}

/**
 *
 */
bool generate_pjl(LaserJob *job, FILE *pjl_file) {
	int i;

	/* Print the printer job language header. */
	fprintf(pjl_file, PJL_HEADER, job->pconf->title->data());
	/* Set autofocus on or off. */
	fprintf(pjl_file, PCL_AUTOFOCUS, job->lconf->focus);
	/* FIXME unknown purpose. */
	fprintf(pjl_file, PCL_UNKNOWN_BLAFOO);
	/* FIXME unknown purpose. */
	fprintf(pjl_file, PCL_UNKNOWN_BLAFOO2);
	/* Left (long-edge) offset registration.  Adjusts the position of the
	 * logical page across the width of the page.
	 */
	fprintf(pjl_file, PCL_OFF_X, 0);
	/* Top (short-edge) offset registration.  Adjusts the position of the
	 * logical page across the length of the page.
	 */
	fprintf(pjl_file, PCL_OFF_Y, 0);
	/* Resolution of the print. */
	fprintf(pjl_file, PCL_PRINT_RESOLUTION, job->lconf->resolution);
	/* X position = 0 */
	fprintf(pjl_file, PCL_POS_X, 0);
	/* Y position = 0 */
	fprintf(pjl_file, PCL_POS_Y, 0);
	/* PCL resolution. */
	fprintf(pjl_file, PCL_RESOLUTION, job->lconf->resolution);

	/* If raster power is enabled and raster mode is not 'n' then add that
	 * information to the print job.
	 */

//	if (job->lconf->raster_power && job->lconf->raster_mode != 'n') {
		/* We're going to perform a raster print. */
//		generate_raster(job->pconf, job->lconf, bitmap_file);
//	}

	/* If vector power is > 0 then add vector information to the print job. */
	if (job->lconf->vector_power) {
		fprintf(pjl_file, R_ORIENTATION, 0);
		fprintf(pjl_file, R_POWER, 50);
		fprintf(pjl_file, R_SPEED, 50);
		fprintf(pjl_file, PCL_UNKNOWN_BLAFOO3);
		fprintf(pjl_file, R_HEIGHT, job->lconf->height * job->lconf->y_repeat);
		fprintf(pjl_file, R_WIDTH, job->lconf->width * job->lconf->x_repeat);

		/* seems to be obsolete, but windows driver does it*/
		fprintf(pjl_file, R_COMPRESSION);
		fprintf(pjl_file, PCL_SECTION_END);

		/* We're going to perform a vector print. */
		list<LaserPass*> passes = job->getPasses();

		list<LaserPass*>::iterator it;
		VectorPass *vpass;
		for (it = passes.begin(); it != passes.end(); it++) {
			vpass = dynamic_cast<VectorPass*> (*it);
			if (vpass) {
				generate_vector(vpass, pjl_file);
			}
		}

	}

	fprintf(pjl_file, PCL_RESET);
	fprintf(pjl_file, PCL_EXIT);
	fprintf(pjl_file, PJL_FOOTER);

	/* Pad out the remainder of the file with 0 characters. */
	for (i = 0; i < 4096; i++) {
		fputc(0, pjl_file);
	}
	return true;
}

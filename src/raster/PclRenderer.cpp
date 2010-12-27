/*
 * Renderer.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: amir hassan
 */

#include "PclRenderer.h"

using boost::format;

PclRenderer::PclRenderer(LaserConfig *lconf) {
	this->lconf = lconf;
}

PclRenderer::~PclRenderer() {
}

void PclRenderer::renderRaster(Raster* raster, ostream& out) {
	list<MMapImage*>::iterator it;
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
		renderTile(*it, out);
	}
	out << "\e*rC"; // end raster
	out << "\26" << "\4"; // some end of file markers
}

void PclRenderer::renderTile(MMapImage* tile, ostream& out) {
	//TODO handle debug flag properly
	char debug = 1;
	int h;
	int d;
	int offx;
	int offy;
	int repeat;

	repeat = this->lconf->raster_repeat;
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

		lconf->width = tile->width();
		lconf->height = tile->height();

		char buf[102400];

		if (debug) {
			fprintf(stderr, "Width %d Height %d Bytes %d Line %d\n",
					lconf->width, lconf->height, h, d);
		}

		for (offx = lconf->width * (lconf->x_repeat - 1); offx >= 0; offx
				-= lconf->width) {
			for (offy = lconf->height * (lconf->y_repeat - 1); offy >= 0; offy
					-= lconf->height) {
				for (pass = 0; pass < passes; pass++) {
					// raster (basic)
					int y;
					char dir = 0;

					for (y = lconf->height - 1; y >= 0; y--) {
						int l;

						switch (lconf->raster_mode) {
						case 'c': // colour (passes)
						{
						}
							break;
						case 'g': // grey level
						{
							for (int x = 0; x < lconf->width; x++) {
								buf[l] = (255 - (uint8_t) tile->pixel({x, y}));
							}
						}
							break;
						default: // mono
						{
							for (int x = 0; x < lconf->width; x++) {
								buf[l] = tile->pixel({x, y});
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
							char pack[sizeof(buf) * 5 / 4 + 1];
							for (r = h - 1; r > l && !buf[r]; r--) {
								;
							}
							r++;
							out << format(PCL_POS_Y) % (tile->offsetY()
									+ lconf->basey + offy + y);
							out << format(PCL_POS_X) % (tile->offsetX()
									+ lconf->basex + offx
									+ ((lconf->raster_mode == 'c'
											|| lconf->raster_mode == 'g') ? l
											: l * 8));
							if (dir) {
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
}

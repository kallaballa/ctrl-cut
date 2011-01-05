/*
 * EpilogCUPS - A laser cutter CUPS driver
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

#include "TPFilter.h"

void TPFilter::filter(Raster *raster) {
	Pixel<unsigned char>* pixel = 0;

	list<DownSample*>::iterator it_tp;
	DownSample * tp;
	int width = raster->sourceImage->width();
	int height = raster->sourceImage->height();

	for (int j = 0; j < height; j++) {
		for (int i = 0; i < width; i++) {
			pixel = (*raster->sourceImage).pixel(Point2D(i, j));
			if (pixel->rgb[0] < 255 || pixel->rgb[1] < 255 || pixel->rgb[2] < 255) {
				Point2D m(i, j);
				bool added = false;
				for (it_tp = raster->grids.begin(); it_tp != raster->grids.end(); it_tp++) {
					tp = *it_tp;

					if ((added = tp->sample(&m)))
						break;
				}

				if (!added) {
					DownSample *tmnew = new DownSample(&m, 25, 25, 5, 5, 2);
					raster->grids.push_back(tmnew);
				}
			}
		}
	}
}

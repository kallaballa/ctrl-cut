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

#ifndef PPMFILE_H_
#define PPMFILE_H_

#define PPMREADBUFLEN 256

#include "stdio.h"
#include <fstream>
#include "util/Logger.h"
#include "MMapMatrix.h"

using namespace std;

Image* loadppm(string filename) {
  LOG_DEBUG_MSG("loading: ", filename);

  char buf[PPMREADBUFLEN], *t;
	unsigned int w, h, d;
	int r;
	int data_offset = 0;
	FILE* pf = fopen(filename.c_str(), "r");

	if (pf == NULL)
		return NULL;
	t = fgets(buf, PPMREADBUFLEN, pf);
	if ((t == NULL) || (strncmp(buf, "P6\n", 3) != 0))
		return NULL;
	data_offset += 4;

	do { /* Px formats can have # comments after first line */
		t = fgets(buf, PPMREADBUFLEN, pf);
		if (t == NULL)
			return NULL;
		data_offset += strlen(buf);
	} while (strncmp(buf, "#", 1) == 0);
	r = sscanf(buf, "%u %u", &w, &h);

	if (r < 2)
		return NULL;
	// The program fails if the first byte of the image is equal to 32. because
	// the fscanf eats the space and the image is read with some bit less
	r = fscanf(pf, "%u\n", &d);
	data_offset += 3;
	if ((r < 1) || (d != 255))
		return NULL;
	fpos_t pos;

	fgetpos(pf, &pos);

	return new Image(filename, w, h, 0, 0, data_offset);
}

#endif /* PPMFILE_H_ */

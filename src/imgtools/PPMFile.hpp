
#ifndef PPMFILE_H_
#define PPMFILE_H_

#define PPMREADBUFLEN 256

#include "stdio.hpp"
#include <fstream>
#include "MMapMatrix.hpp"

using namespace std;

Image* loadppm(string filename) {
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

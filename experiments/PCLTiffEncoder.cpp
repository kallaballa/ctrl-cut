/* $Id: tiffinfo.c,v 1.7 2004/09/03 08:19:27 dron Exp $ */

/*
 * Copyright (c) 1988-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#include "tiffconf.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_STRINGS_H
# include <strings.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#include "tiffio.h"

#define	streq(a,b)	(strcasecmp(a,b) == 0)

int showdata = 0; /* show data */
int rawdata = 0; /* show raw/decoded data */
int showwords = 0; /* show data as bytes/words */
int readdata = 0; /* read data in file */
int stoponerr = 1; /* stop on first read error */

static void usage(void);
static void tiffinfo(TIFF*, uint16, long);

int main(int argc, char* argv[]) {
	int dirnum = -1, multiplefiles, c;
	uint16 order = 0;
	TIFF* tif;
	extern int optind;
	extern char* optarg;
	long flags = 0;
	uint32 diroff = 0;
	int chopstrips = 0; /* disable strip chopping */

	while ((c = getopt(argc, argv, "f:o:cdDSjilmrsvwz0123456789")) != -1)
		switch (c) {
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			dirnum = atoi(&argv[optind - 1][1]);
			break;
		case 'd':
			showdata++;
			/* fall thru... */
		case 'D':
			readdata++;
			break;
		case 'c':
			flags |= TIFFPRINT_COLORMAP | TIFFPRINT_CURVES;
			break;
		case 'f': /* fill order */
			if (streq(optarg, "lsb2msb"))
				order = FILLORDER_LSB2MSB;
			else if (streq(optarg, "msb2lsb"))
				order = FILLORDER_MSB2LSB;
			else
				usage();
			break;
		case 'i':
			stoponerr = 0;
			break;
		case 'o':
			diroff = strtoul(optarg, NULL, 0);
			break;
		case 'j':
			flags |= TIFFPRINT_JPEGQTABLES | TIFFPRINT_JPEGACTABLES
					| TIFFPRINT_JPEGDCTABLES;
			break;
		case 'r':
			rawdata = 1;
			break;
		case 's':
			flags |= TIFFPRINT_STRIPS;
			break;
		case 'w':
			showwords = 1;
			break;
		case 'z':
			chopstrips = 1;
			break;
		case '?':
			usage();
			/*NOTREACHED*/
		}
	if (optind >= argc)
		usage();
	multiplefiles = (argc - optind > 1);
	for (; optind < argc; optind++) {
		if (multiplefiles)
			printf("%s:\n", argv[optind]);
		tif = TIFFOpen(argv[optind], chopstrips ? "rC" : "rc");
		if (tif != NULL) {
			if (dirnum != -1) {
				if (TIFFSetDirectory(tif, (tdir_t) dirnum))
					tiffinfo(tif, order, flags);
			} else if (diroff != 0) {
				if (TIFFSetSubDirectory(tif, diroff))
					tiffinfo(tif, order, flags);
			} else {
				do
					tiffinfo(tif, order, flags);
				while (TIFFReadDirectory(tif));
			}
			TIFFClose(tif);
		}
	}
	return (0);
}

char* stuff[] = { "usage: tiffinfo [options] input...", "where options are:",
		" -D		read data", " -i		ignore read errors",
		" -c		display data for grey/color response curve or colormap",
		" -d		display raw/decoded image data",
		" -f lsb2msb	force lsb-to-msb FillOrder for input",
		" -f msb2lsb	force msb-to-lsb FillOrder for input",
		" -j		show JPEG tables", " -o offset	set initial directory offset",
		" -r		read/display raw image data instead of decoded data",
		" -s		display strip offsets and byte counts",
		" -w		display raw data in words rather than bytes",
		" -z		enable strip chopping",
		" -#		set initial directory (first directory is # 0)", NULL };

static void usage(void) {
	char buf[BUFSIZ];
	int i;

	setbuf(stderr, buf);
	fprintf(stderr, "%s\n\n", TIFFGetVersion());
	for (i = 0; stuff[i] != NULL; i++)
		fprintf(stderr, "%s\n", stuff[i]);
	exit(-1);
}

static void ShowRawBytes(unsigned char* pp, uint32 n) {
	uint32 i;

	for (i = 0; i < n; i++) {
		printf(" %02x", *pp++);
		if (((i + 1) % 24) == 0)
			printf("\n ");
	}
	putchar('\n');
}

static void pclEncode(unsigned char* pp, uint32 scanlineSize, uint32 n) {
	uint32 i;
	int cnt = 0;
	signed char* compressedSL = _TIFFmalloc(scanlineSize + 1);
	signed char header;
	size_t rawSize = 0, compSize = 0, runlength;

	for (i = 0; i < n; i++) {
		header = *pp++;

		if (header > -128) {
			if (header >= 0) {
				//literal data
				runlength = 2 + header;
				rawSize += runlength - 1;
				compSize += runlength;

				_TIFFmemcpy(pp, compressedSL, runlength);
				*compressedSL += runlength;
			} else {
				//rle data
				runlength  =  2;
				rawSize +=  1 - header;
				compSize += runlength;

				_TIFFmemcpy(pp, compressedSL, runlength);
				*compressedSL += runlength;
			}
		}

		if(rawSize >= scanlineSize)	{
			printf("b%dW", compSize);
			*compressedSL-=compSize;

			for(int j = 0; j < compSize; j++)	{

				printf(" %02x", *compressedSL++);
			}

			rawSize = 0;
			compSize = 0;
		}

		if(rawSize >= scanlineSize)
			printf("ouch");
	}

	putchar('\n');
}

void TIFFReadRawData(TIFF* tif, int bitrev) {
	tstrip_t nstrips = TIFFNumberOfStrips(tif);
	const char* what = TIFFIsTiled(tif) ? "Tile" : "Strip";
	uint32* stripbc;

	TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &stripbc);
	if (nstrips > 0) {
		uint32 bufsize = stripbc[0];
		tdata_t buf = _TIFFmalloc(bufsize);
		tstrip_t s;

		for (s = 0; s < nstrips; s++) {
			if (stripbc[s] > bufsize) {
				buf = _TIFFrealloc(buf, stripbc[s]);
				bufsize = stripbc[s];
			}
			if (buf == NULL) {
				fprintf(stderr, "Cannot allocate buffer to read strip %lu\n",
						(unsigned long) s);
				break;
			}
			if (TIFFReadRawStrip(tif, s, buf, stripbc[s]) < 0) {
				fprintf(stderr, "Error reading strip %lu\n", (unsigned long) s);
				if (stoponerr)
					break;
			} else if (showdata) {
				if (bitrev) {
					TIFFReverseBits(buf, stripbc[s]);
				}
				printf("%s %lu:\n ", what, (unsigned long) s);

				pclEncode ((unsigned char*) buf,TIFFScanlineSize(tif), stripbc[s]);
			}
		}
		if (buf != NULL)
			_TIFFfree(buf);
	}
}

static void tiffinfo(TIFF* tif, uint16 order, long flags) {
	TIFFPrintDirectory(tif, stdout, flags);

	if (rawdata) {
		if (order) {
			uint16 o;
			TIFFGetFieldDefaulted(tif, TIFFTAG_FILLORDER, &o);
			TIFFReadRawData(tif, o != order);
		} else
			TIFFReadRawData(tif, 0);
	}
}

/* vim: set ts=8 sts=8 sw=8 noet: */

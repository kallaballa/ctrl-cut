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

#ifndef SIGNATURES_H_
#define SIGNATURES_H_

#define PCL_RASTER_START "*rA"
#define PCL_RASTER_END "*rC"
#define PCL_FLIPY "&yO"
#define PCL_X "*pX"
#define PCL_Y "*pY"
#define PCL_WIDTH "*rS"
#define PCL_HEIGHT "*rT"
#define PCL_RLE_DATA "*bW"
#define PCL_PIXEL_LEN "*bA"
#define PCL_END_OF_SETTINGS "*rA"
#define PCL_END_OF_INSTRUCTION 0x1b

#endif /* SIGNATURES_H_ */

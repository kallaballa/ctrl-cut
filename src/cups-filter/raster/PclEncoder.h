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

#ifndef PCL_RENDERER_H_
#define PCL_RENDERER_H_

#include <map>

#include "util/LaserConfig.h"
#include "util/PJL.h"
#include "RTypes.h"
#include "Raster.h"
#include "MMapMatrix.h"

class PclEncoder {

public:
	LaserConfig *lconf;

	void encode(Raster *raster, ostream &out);
	void encodeTile(Image* tile, ostream& out);

	PclEncoder(LaserConfig *lconf);
	virtual ~PclEncoder();
};

#endif /* PCL_RENDERER_H_ */

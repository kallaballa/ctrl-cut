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
#ifndef R_TYPES_H_
#define R_TYPES_H_

/** Number of bytes in the bitmap header. */
#define BITMAP_HEADER_NBYTES (54)

#include <string>
#include <iostream>

using namespace std;

struct Point2D {
	Point2D(int x, int y) :
		x(x), y(y) {
	}
	int x, y;
};

template<class T>
class Pixel {
public:
	Pixel(T r, T g, T b) {
		this->rgb[0] = r;
		this->rgb[1] = g;
		this->rgb[2] = b;
		initIHS(this->rgb);
	}

	T rgb[3];
	T ihs[3];

private:
	void initIHS(T* rgb) {
		T r = rgb[0];
		T g = rgb[1];
		T b = rgb[2];
		T i, s, h;
		if (r == 0 && g == 0 && b == 0) {
			i = 0;
			h = 0;
			s = 0;
		} else {
			i = (r + g + b) / 3;
			s = 1 - 3 * (min(min(r, g), b)) / (r + g + b);
			h = acos(0.5 * ((r - g) + (r - b))) / sqrt(pow((r - g), 2) + (r - b) * (g
					- b));
		}
		if (b > g)
			h = 2.0 * 3.14159265 - h;
		this->ihs[0] = i;
		this->ihs[1] = h;
		this->ihs[2] = s;
	}
};

class Rectangle {
public:
	int ul_x, ul_y, lr_x, lr_y, width, height;

	Rectangle(int ul_x = 0, int ul_y = 0, int lr_x = 0, int lr_y = 0) {
		this->ul_x = ul_x;
		this->ul_y = ul_y;
		this->lr_x = lr_x;
		this->lr_y = lr_y;

		this->width = lr_x - ul_x;
		this->height = lr_y - ul_y;
	}

	bool inside(Point2D* m, int tolerance = 0) {
		return (m->x < (lr_x + tolerance) && m->x > (ul_x - tolerance) && m->y
				< (lr_y + tolerance) && m->y > (ul_y - tolerance));
	}
};

#endif /* R_TYPES_H_ */
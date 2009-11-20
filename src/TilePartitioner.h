/*
 * TilePartitioner.h
 *
 *  Created on: 25.10.2009
 *      Author: amir
 */

#ifndef TILEPARTITIONER_H_
#define TILEPARTITIONER_H_

#include "boost/multi_array.hpp"
#include <list>
#include "Raster.h"

using namespace std;

class BBox: public Rectangle {
public:
	bool inside(int x, int y, int tol_x = 0, int tol_y = 0) {
		return (x < (lr_x + tol_x) && x > (ul_x - tol_x) && y < (lr_y + tol_y)
				&& y > (ul_y - tol_y));
	}

	void adjustTo(Marker* m) {
		this->ul_x = min(m->x, ul_x);
		this->ul_y = min(m->y, ul_y);
		this->lr_x = max(m->x, lr_x);
		this->lr_y = max(m->y, lr_y);
	}

	void adjustTo(Rectangle* r) {
		this->ul_x = min(r->ul_x, ul_x);
		this->ul_y = min(r->ul_y, ul_y);
		this->lr_x = max(r->lr_x, lr_x);
		this->lr_y = max(r->lr_y, lr_y);
	}
};

typedef Rectangle Cell;

class TilePartitioner {
	typedef boost::multi_array<Cell*, 2> CellGrid;
	CellGrid* cells;
	list<Cell*> cellList;
	BBox boundingBox;
	int tolerance;

	int cell_width, cell_height, num_cells_x, num_cells_y;
public:

	TilePartitioner(Marker* m_ul, int num_cells_x, int num_cells_y,
			int cell_width, int cell_height, int tolerance);
	virtual ~TilePartitioner();
	bool mark(Marker* m);
	BBox getBoundingBox();
	void resizeGrid(int width, int height);
	list<Cell*> getMarkedCells();
};

#endif /* TILEPARTITIONER_H_ */

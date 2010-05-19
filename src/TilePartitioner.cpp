/*
 * TilePartitioner.cpp
 *
 *  Created on: 25.10.2009
 *      Author: amir
 */
#include "boost/multi_array.hpp"
#include <algorithm>
#include <list>
#include "TilePartitioner.h"
#include <iostream>

using namespace std;
TilePartitioner::TilePartitioner(Marker* m_ul, int num_cells_x,
                                 int num_cells_y, int cell_width, int cell_height, int tolerance) {
  this->num_cells_x = num_cells_x;
  this->num_cells_y = num_cells_y;

  this->cell_width = cell_width;
  this->cell_height = cell_height;
  this->tolerance = tolerance;

  this->boundingBox.ul_x = m_ul->x;
  this->boundingBox.ul_y = m_ul->y;
  this->boundingBox.lr_x = m_ul->x + cell_width;
  this->boundingBox.lr_y = m_ul->y + cell_height;

  this->cells = NULL;
}

TilePartitioner::~TilePartitioner() {
  // TODO Auto-generated destructor stub
}

bool TilePartitioner::mark(Marker* m) {
  if (!boundingBox.inside(m->x, m->y, cell_width * tolerance, cell_height
                          * tolerance))
    return false;
  else {
    int cx = m->x / cell_width;
    int cy = m->y / cell_height;

    //TODO figure appropriate growth rate
    if (cells == NULL || cx >= this->num_cells_x || cy >= this->num_cells_y) {
      int ncx = max(this->num_cells_x, (cx));
      int ncy = max(this->num_cells_y, (cy));
      resizeGrid(ncx * 2, ncy * 2);
    }

    Cell* c = (*cells)[cx][cy];

    if (c == NULL) {
      int ul_x = cx * cell_width;
      int ul_y = cy * cell_height;
      int lr_x = ul_x + cell_width;
      int lr_y = ul_y + cell_height;
      c = new Cell(ul_x, ul_y, lr_x, lr_y);
      (*cells)[cx][cy] = c;
    }

    boundingBox.adjustTo(c);
    cellList.push_back(c);
    return true;
  }
}

void TilePartitioner::resizeGrid(int num_x, int num_y) {
  int offset = 0;
  if (cells == NULL)
    cells = new CellGrid(boost::extents[num_x][num_y]);
  else {
    offset = this->num_cells_x * this->num_cells_y;
    cells->resize(boost::extents[num_x][num_y]);
  }

  std::fill(cells->data() + offset, cells->data() + cells->num_elements(),
            (Cell*) NULL);

  this->num_cells_x = num_x;
  this->num_cells_y = num_y;
}

list<Cell*> TilePartitioner::getMarkedCells() {
  return cellList;
}

BBox TilePartitioner::getBoundingBox() {
  return this->boundingBox;
}

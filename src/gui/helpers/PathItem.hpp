/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef PATHITEM_H_
#define PATHITEM_H_

#include "cut/Cut.hpp"
#include <QGraphicsPathItem>

class PathItem: public QGraphicsPathItem {
public:
  const Path& path;

  PathItem(const Path& path, QGraphicsItem *parent = 0);
  ~PathItem(){};

  void commit() {
  }
};

#endif /* PATHITEM_H_ */

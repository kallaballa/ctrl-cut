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
 * GNU General Public License for more de0tails.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef CUTITEM_H_
#define CUTITEM_H_

#include "cut/model/Cut.hpp"
#include "config/CutSettings.hpp"
#include "Qt.h"

class CutItem: public AbstractCtrlCutItem {
public:
  CutModel& cut;

  CutItem(CutModel& cut);
  ~CutItem(){};

  void commit() {
    const QPointF& pos = this->pos();
    this->cut.put(CutSettings::CPOS,Point(pos.x(), pos.y()));
  }
};

#endif /* CUTITEM_H_ */

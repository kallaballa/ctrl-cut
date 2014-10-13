/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */


#ifndef ENGRAVINGITEM_H_
#define ENGRAVINGITEM_H_

#include "engrave/Engrave.hpp"
#include "config/EngraveSettings.hpp"
#include "GraphicsItems.hpp"

class EngraveItem: public AbstractCtrlCutItem {
public:
  EngravingPtr engraving;

  EngraveItem(EngravingPtr engraving);
  EngraveItem(const EngraveItem& engraveItem);
  virtual ~EngraveItem(){};

  void init();

  void commit() {
    const QPointF& pos = this->scenePos();
    this->engraving->settings.put(EngraveSettings::EPOS,Point(pos.x(), pos.y()));
  }
};

#endif /* ENGRAVINGITEM_H_ */

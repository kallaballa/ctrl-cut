#ifndef CUTITEM_H_
#define CUTITEM_H_

#include "cut/Cut.hpp"
#include "config/CutSettings.hpp"
#include "GraphicsItems.hpp"

class CutItem: public AbstractCtrlCutItem {
public:
  CutPtr cut;

  CutItem(CutPtr cut);
  CutItem(const CutItem& cutItem);
  virtual ~CutItem(){};

  void init();

  void commit() {
    const QPointF& pos = this->scenePos();
    this->cut->put(CutSettings::CPOS,Point(pos.x(), pos.y()));
  }
};

#endif /* CUTITEM_H_ */

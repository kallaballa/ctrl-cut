#include "CutItem.hpp"
#include "PathItem.hpp"
#include "cut/operations/Translate.hpp"
#include <qgraphicsitem.h>
#include <boost/foreach.hpp>

CutItem::CutItem(CutPtr cut) : AbstractCtrlCutItem(), cut(cut) {
  init();
}

CutItem::CutItem(const CutItem& cutItem) : AbstractCtrlCutItem(), cut(CutPtr(new Cut(cutItem.cut->settings))) {
  (*cut.get()) = *cutItem.cut.get();
  init();
}

void CutItem::init() {
  QGraphicsItemGroup::setFlags(ItemIsSelectable | ItemIsMovable);
  this->cut->crop();
  Point pos = this->cut->get(CutSettings::CPOS);
  setPos(QPointF(pos.x, pos.y));

  BOOST_FOREACH(const Path& p, *cut) {
    PathItem* pi = new PathItem(p);
    pi->setPos(QPointF(pos.x, pos.y));
    this->addToGroup(pi);
  }
}

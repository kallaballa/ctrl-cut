/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "CtrlCutScene.hpp"
#include "helpers/CutItem.hpp"
#include "helpers/EngraveItem.hpp"
#include "ctrl-cut/config/DocumentSettings.hpp"
#include "ctrl-cut/cut/geom/Geometry.hpp"
#include "Commands.hpp"
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include "helpers/DocumentHolder.hpp"
#include <QGraphicsItem>
#include <boost/filesystem.hpp>
#include <cut/operations/NearestPathSorting.hpp>
#include <qpixmapcache.h>
#include <qpainter.h>
#include <qvarlengtharray.h>
#include <qmenu.h>
#include <qgraphicsview.h>
#include <algorithm>
#include "NewDialog.hpp"
#include "MainWindow.hpp"
#include "helpers/Qt.hpp"

CtrlCutScene::CtrlCutScene(QObject *parent) :
  QGraphicsScene(parent) {
  this->docHolder = DocumentHolderPtr(new DocumentHolder());
  this->docHolder->doc = NULL;
  this->laserbed = NULL;
  this->backgroundItem = NULL;
  this->currentZ = 0;
  this->setBackgroundBrush(Qt::NoBrush);
  setItemIndexMethod(QGraphicsScene::BspTreeIndex);

  MainWindow* mainw = qobject_cast<MainWindow*>(this->parent());

  this->makeBackground();

  using namespace Qt;


  /*
   typedef CtrlCutEvent CCE;
   typedef CCE::MoveItems MoveItems;
   typedef CCE::GroupItems GroupItems;
   typedef CCE::UngroupItems UngroupItems;

   qreal step = 1;
   CCE::registerAction(ControlModifier, Key_Up, MoveItems::UP(step));
   CCE::registerAction(ControlModifier, Key_Left, MoveItems::LEFT(step));
   CCE::registerAction(ControlModifier, Key_Down, MoveItems::DOWN(step));
   CCE::registerAction(ControlModifier, Key_Right, MoveItems::RIGHT(step));

   step = 10;
   CCE::registerAction(Key_Up, MoveItems::UP(step));
   CCE::registerAction(Key_Left, MoveItems::LEFT(step));
   CCE::registerAction(Key_Down, MoveItems::DOWN(step));
   CCE::registerAction(Key_Right, MoveItems::RIGHT(step));

   step = 100;
   CCE::registerAction(ShiftModifier, Key_Up, MoveItems::UP(step));
   CCE::registerAction(ShiftModifier, Key_Left, MoveItems::LEFT(step));
   CCE::registerAction(ShiftModifier, Key_Down, MoveItems::DOWN(step));
   CCE::registerAction(ShiftModifier, Key_Right, MoveItems::RIGHT(step));

   CCE::registerAction(ControlModifier, Key_G, GroupItems(*this));
   CCE::registerAction(ShiftModifier, Key_G, UngroupItems(*this));*/
}

/*
 void CtrlCutScene::keyPressEvent(QKeyEvent *event) {
 CtrlCutEvent::act(*event, this->selectedItems());
 }*/

void CtrlCutScene::mousePressEvent(QGraphicsSceneMouseEvent *event) {

  // Call QGraphicsScene's event handler first to deal with selection
  QGraphicsScene::mousePressEvent(event);

  QPointF mousePos(event->buttonDownScenePos(Qt::LeftButton).x(),
      event->buttonDownScenePos(Qt::LeftButton).y());
  //  std::cerr << "press: " << mousePos.x() << ", " << mousePos.y() << std::endl;
  QList<QGraphicsItem* > selected = selectedItems();
  if (!selected.empty()) {
    movingItem = selected.first();

    if (movingItem != 0 && event->button() == Qt::LeftButton) {
      //      std::cerr << "press: " << movingItem->pos().x() << ", "
      //          << movingItem->pos().y() << std::endl;
      oldPos = movingItem->pos();
    }

    std::cerr << std::endl;
  } else {
    movingItem = NULL;
  }
}

void CtrlCutScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  QPointF mousePos(event->buttonDownScenePos(Qt::LeftButton).x(),
      event->buttonDownScenePos(Qt::LeftButton).y());
  //  std::cerr << "release: " << mousePos.x() << ", " << mousePos.y() << std::endl;
  //  std::cerr << "movingItem: " << movingItem << std::endl;
  //  std::cerr << "button: " << event->button() << std::endl;
  if (movingItem != 0 && event->button() == Qt::LeftButton) {
    //    std::cerr << "release: " << movingItem->pos().x() << ", " << movingItem->pos().y() << std::endl;
    if (oldPos != movingItem->pos())
      emit itemMoved(movingItem, oldPos);
    movingItem = 0;
  }
  std::cerr << std::endl;
  QGraphicsScene::mouseReleaseEvent(event);
}

void CtrlCutScene::open(const QString& filename) {
  this->reset();
  load(filename);
}

void CtrlCutScene::newJob(const Coord_t resolution, const Distance width, const Distance height, const QString title) {
  typedef DocumentSettings DS;
  this->reset();
  if(this->docHolder->doc == NULL)
    this->docHolder->doc = DocumentPtr(new Document());

  this->docHolder->doc->put(DS::TITLE, title.toStdString());
  this->docHolder->doc->put(DS::RESOLUTION, resolution);
  this->docHolder->doc->put(DS::WIDTH, width);
  this->docHolder->doc->put(DS::HEIGHT, height);
  if(this->backgroundItem)
    delete this->backgroundItem;

  this->backgroundItem = NULL;
  makeBackground();
}

std::vector<AbstractCtrlCutItem*> CtrlCutScene::load(const QString filename, bool loadVector, bool loadRaster) {
  std::vector<AbstractCtrlCutItem*> items;
  if(filename.endsWith(".svg", Qt::CaseInsensitive)) {
    showWarningDialog("SVG text elements currently not supported. Please make sure all text is converted to paths or use the eps file format.","");
  }

  try {
    if(!this->docHolder->doc)
      newJob(1, Distance(), Distance(), "untitled");
    DocumentPtr doc = this->docHolder->doc;

    doc->put(DocumentSettings::LOAD_CUT, loadVector);
    doc->put(DocumentSettings::LOAD_ENGRAVING, loadRaster);
    doc->put(EngraveSettings::DITHERING, EngraveSettings::FLOYD_STEINBERG);

    std::pair<Document::CutList, Document::EngraveList> loaded = doc->load(filename.toStdString());
    makeBackground();

    qreal width = doc->get(DocumentSettings::WIDTH).in(PX);
    qreal height = doc->get(DocumentSettings::HEIGHT).in(PX);
    QPixmapCache::setCacheLimit((width * height) / 8 * 2);

    for(Document::CutConstIt it = loaded.first.begin(); it != loaded.first.end(); ++it) {
      CutItem* ci = new CutItem(*it);
      ci->setZValue(++this->currentZ);
      // Don't add to document as it's already there
      this->docHolder->addItem(*ci);
      this->addItem(ci);
      items.push_back(ci);
    }

    const Document::EngraveList& engravings = doc->engravings();
    for(Document::EngraveConstIt it = loaded.second.begin(); it != loaded.second.end(); ++it) {
      EngraveItem* ei = new EngraveItem(*it);
      ei->setZValue(++this->currentZ);
      // Don't add to document as it's already there
      this->docHolder->addItem(*ei);
      this->addItem(ei);
      items.push_back(ei);
    }

    this->views()[0]->setSceneRect(QRectF(width/-4, height/-4, width * 1.5, height * 1.5));
  } catch(std::exception& ex) {
    showErrorDialog("Unable to load file:" + filename.toStdString(), ex.what());
  }
  return items;
}

void CtrlCutScene::add(AbstractCtrlCutItem &item) {
  this->docHolder->add(item);
  this->addItem(&item);
}

void CtrlCutScene::remove(AbstractCtrlCutItem &item) {
  this->docHolder->remove(item);
  this->removeItem(&item);
}

void CtrlCutScene::attachDocumentHolder(DocumentHolderPtr docHolder) {
  this->detachDocumentHolder();
  this->docHolder = docHolder;

  foreach(AbstractCtrlCutItem *item, this->docHolder->items) {
    this->addItem(item);
  }
}

void CtrlCutScene::detachDocumentHolder() {
  foreach(AbstractCtrlCutItem *item, docHolder->items) {
    this->removeItem(item);
  }
}

void CtrlCutScene::reset() {
  this->detachDocumentHolder();

  if(this->docHolder->doc != NULL)
    this->docHolder->doc->clear();

  this->docHolder->items.clear();
}

void CtrlCutScene::drawBackground( QPainter * painter, const QRectF & rect ) {
  uint32_t width;
  uint32_t height;
  uint32_t resolution;

  if(docHolder->doc != NULL) {
    width = docHolder->doc->get(DocumentSettings::WIDTH).in(PX);
    height = docHolder->doc->get(DocumentSettings::HEIGHT).in(PX);
    resolution = docHolder->doc->get(DocumentSettings::RESOLUTION);

    painter->setPen(Qt::black);
    painter->fillRect(QRect(QPoint(0, 0), QSize(width, height)), QBrush(Qt::gray));
    painter->setPen(Qt::lightGray);
    uint32_t cellsize = Distance(5,MM,resolution).in(PX);
    QVarLengthArray<QLineF, 100> lines;

    for (int j = cellsize; j < height; j += cellsize) {
      lines.append(QLineF(0, j, width, j));
    }

    for (int i = cellsize;  i < width; i += cellsize) {
      lines.append(QLineF(i, 0, i, height));
    }

    painter->drawLines(lines.data(), lines.size());
  }
}

void CtrlCutScene::update(const QRectF &rect) {
  foreach (QGraphicsItem *sitem, this->items()) {
    AbstractCtrlCutItem* ccItem;
    if ((ccItem = dynamic_cast<AbstractCtrlCutItem*> (sitem->parentItem()))) {
      ccItem->commit();
    }
  }

  QGraphicsScene::update(rect);
}

void CtrlCutScene::makeBackground() {
  if(docHolder->doc != NULL) {
    if(backgroundItem == NULL) {
        backgroundItem = new QGraphicsPolygonItem();
        this->addItem(backgroundItem);
    }
    QPolygon polygon;
    uint32_t width = docHolder->doc->get(DocumentSettings::WIDTH).in(PX);
    uint32_t height = docHolder->doc->get(DocumentSettings::HEIGHT).in(PX);

    QPen p(Qt::blue);
    polygon << QPoint(0, 0) << QPoint(width, 0) << QPoint(width, height) << QPoint(0, height) << QPoint(0,0);
    backgroundItem->setPolygon(polygon);
    backgroundItem->setPen(p);
    backgroundItem->setZValue(-9999);
  }
}




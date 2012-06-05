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
#include "CtrlCutScene.hpp"
#include "Document.hpp"
#include "helpers/CutItem.hpp"
#include "helpers/EngraveItem.hpp"
#include "event/CtrlCutEvent.hpp"
#include "ctrl-cut/config/DocumentSettings.hpp"
#include "ctrl-cut/cut/geom/Geometry.hpp"
#include "Commands.hpp"
#include <QGraphicsItem>
#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>
#include "helpers/DocumentHolder.hpp"
#include <QGraphicsItem>
#include "cut/model/Clip.hpp"
#include "cut/model/Explode.hpp"
#include "cut/model/Deonion.hpp"
#include "cut/model/Reduce.hpp"
#include "cut/model/NearestPathSorting.h"
#include "cut/graph/Planar.hpp"
#include "cut/model/SvgPlot.hpp"
#include <boost/filesystem.hpp>
#include <qpixmapcache.h>
#include <qpainter.h>
#include <qvarlengtharray.h>

CtrlCutScene::CtrlCutScene(QObject *parent) :
  QGraphicsScene(parent) {
  this->docHolder.doc = new Document();
  this->laserbed = NULL;
  setItemIndexMethod(QGraphicsScene::BspTreeIndex);

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
  QPointF mousePos(event->buttonDownScenePos(Qt::LeftButton).x(),
      event->buttonDownScenePos(Qt::LeftButton).y());
  std::cerr << "press: " << mousePos.x() << ", " << mousePos.y() << std::endl;
  QList<QGraphicsItem* > selected = selectedItems();
  if (!selected.empty()) {
    movingItem = selected.first();

    if (movingItem != 0 && event->button() == Qt::LeftButton) {
      std::cerr << "press: " << movingItem->pos().x() << ", "
          << movingItem->pos().y() << std::endl;
      oldPos = movingItem->pos();
    }

    std::cerr << std::endl;
  } else {
    movingItem = NULL;
  }
  QGraphicsScene::mousePressEvent(event);
}

void CtrlCutScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
  QPointF mousePos(event->buttonDownScenePos(Qt::LeftButton).x(),
      event->buttonDownScenePos(Qt::LeftButton).y());
  std::cerr << "release: " << mousePos.x() << ", " << mousePos.y() << std::endl;

  if (movingItem != 0 && event->button() == Qt::LeftButton) {
    std::cerr << "release: " << movingItem->pos().x() << ", " << movingItem->pos().y() << std::endl;
    if (oldPos != movingItem->pos())
      emit itemMoved(movingItem, oldPos);
    movingItem = 0;
  }
  std::cerr << std::endl;
  QGraphicsScene::mouseReleaseEvent(event);
}

void CtrlCutScene::setDocumentHolder(DocumentHolder& docHolder) {
  this->reset();
  this->docHolder = docHolder;

  foreach(CutItem* ci, docHolder.cutItems)
    {
      this->addItem(ci);
    }

  foreach(EngraveItem* ei, docHolder.engraveItems)
    {
      this->addItem(ei);
    }
}

void CtrlCutScene::open(const QString& filename) {
  this->reset();
  load(filename);
}

void CtrlCutScene::newJob(const QString& title, const Coord_t& resolution, const Distance& width, const Distance& height) {
  typedef DocumentSettings DS;
  this->reset();
  this->docHolder.doc = new Document();
  this->docHolder.doc->put(DS::TITLE, title.toStdString());
  this->docHolder.doc->put(DS::RESOLUTION, resolution);
  this->docHolder.doc->put(DS::WIDTH, width);
  this->docHolder.doc->put(DS::HEIGHT, height);
}

void CtrlCutScene::load(const QString& filename) {
  if (!this->docHolder.doc) {
    this->newJob(boost::filesystem::path(filename.toStdString()).filename().c_str(), 600, Distance(36, IN, 600), Distance(24, IN, 600));
  }
//  QPixmapCache::setCacheLimit(width * height / 8 * 2);
  Document doc;

  //FIXME make settings available through the gui
  doc.put(EngraveSettings::DITHERING, EngraveSettings::BAYER);
  doc.put(DocumentSettings::LOAD_ENGRAVING, true);
  doc.put(DocumentSettings::RESOLUTION, 600);
  doc.put(DocumentSettings::WIDTH, Distance(21600, PX, 600));
  doc.put(DocumentSettings::HEIGHT, Distance(14400, PX, 600));
  doc.load(filename.toStdString());
  string basename = doc.get(DocumentSettings::FILENAME);
  uint32_t resolution = doc.get(DocumentSettings::RESOLUTION);
  uint32_t width = doc.get(DocumentSettings::WIDTH).in(PX);
  uint32_t height = doc.get(DocumentSettings::HEIGHT).in(PX);
  Distance reduceMax = Distance(1, MM, resolution);
  QPixmapCache::setCacheLimit((width * height) / 8 * 2);
  for (Document::CutIt it = doc.begin_cut(); it != doc.end_cut(); it++) {
    Cut& cut = **it;

    plot_svg(cut, basename + "_input");

    Cut clipped = make_from(cut);
    Cut planar = make_from(cut);
    Cut exploded = make_from(cut);
    Cut reduced = make_from(cut);
    Cut sorted = make_from(cut);

    clip(cut, clipped, Box(Point(0,0),Point(width,height)), Distance(30,MM, resolution));
    plot_svg(clipped, basename + "_clipped");

    explode(clipped, exploded, Distance(30,MM, resolution));
    plot_svg(exploded, basename + "_exploded");

    make_planar(exploded, planar);
    plot_svg(planar, basename + "_planar");

    reduce(planar, reduced, reduceMax.in(PX));
    plot_svg(reduced, basename + "_reduced");
/*
    nearest_path_sorting(reduced, sorted);
    plot(sorted, basename + "_sorted");
*/
    traverse_onion(reduced, sorted);
    plot_svg(sorted, basename + "_onion");

    cut.clear();
    cut = sorted;
    plot_svg(cut, basename + "_cut");
    CutItem* ci = new CutItem(**it);
    this->docHolder.add(*ci);
    this->addItem(ci);
  }

  for (Document::EngraveIt it = doc.begin_engrave(); it != doc.end_engrave(); it++) {
    EngraveItem* ei = new EngraveItem(**it);
    this->docHolder.add(*ei);
    this->addItem(ei);
  }
}

void CtrlCutScene::add(CutItem& cutItem) {
  this->docHolder.add(cutItem);
  this->addItem(&cutItem);
}

void CtrlCutScene::remove(CutItem& cutItem) {
  this->docHolder.remove(cutItem);
  this->removeItem(&cutItem);
}

void CtrlCutScene::add(EngraveItem& engraveItem) {
  this->docHolder.add(engraveItem);
  this->addItem(&engraveItem);
}

void CtrlCutScene::remove(EngraveItem& engraveItem) {
  this->docHolder.remove(engraveItem);
  this->removeItem(&engraveItem);
}

void CtrlCutScene::reset() {
  foreach(CutItem* ci, docHolder.cutItems)
    {
      this->removeItem(ci);
    }

  foreach(EngraveItem* ei, docHolder.engraveItems)
    {
      this->removeItem(ei);
    }

  this->docHolder.doc = NULL;

  while (!this->docHolder.cutItems.empty())
    this->docHolder.cutItems.takeFirst();

  while (!this->docHolder.engraveItems.empty())
    this->docHolder.engraveItems.takeFirst();
}

void CtrlCutScene::drawBackground ( QPainter * painter, const QRectF & rect ) {
  uint32_t width;
  uint32_t height;
  uint32_t resolution;

  if(docHolder.doc != NULL) {
    width = docHolder.doc->get(DocumentSettings::WIDTH).in(PX);
    height = docHolder.doc->get(DocumentSettings::HEIGHT).in(PX);
    resolution = docHolder.doc->get(DocumentSettings::RESOLUTION);

    painter->setPen(Qt::black);
    painter->fillRect(QRect(QPoint(0, 0), QSize(width, height)), QBrush(Qt::white));
    painter->setPen(Qt::lightGray);
    uint32_t cellsize = Distance(50,MM,resolution).in(PX);
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

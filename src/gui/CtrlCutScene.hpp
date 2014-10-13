/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef CTRLCUTSCENE_H_
#define CTRLCUTSCENE_H_

// Workaround for https://bugreports.qt-project.org/browse/QTBUG-22829
#ifdef Q_MOC_RUN
#define BOOST_LEXICAL_CAST_INCLUDED
#define BOOST_TT_HAS_OPERATOR_HPP_INCLUDED
#endif

#include <QGraphicsScene>
#include <QObject>
#include <QGraphicsItem>
#include <qevent.h>
#include <memory>
#include "helpers/DocumentHolder.hpp"
#include "cut/geom/Geometry.hpp"
#include "util/Measurement.hpp"
#include "Document.hpp"
#include "config/Settings.hpp"

class CtrlCutScene : public QGraphicsScene
{
  Q_OBJECT

public:
  QList<class AbstractCtrlCutItem *> itemClipboard;
  QList<Settings *> settingsClipboard;

  CtrlCutScene(QObject *parent = 0);
  ~CtrlCutScene() {}

  void attachDocumentHolder(DocumentHolderPtr doc);

  DocumentHolderPtr getDocumentHolder() {
    return this->docHolder;
  }
  void add(class AbstractCtrlCutItem& cutItem);
  void remove(AbstractCtrlCutItem& cutItem);
  void newJob(const Coord_t& resolution, const Distance& width, const Distance& height, const QString& title = "untitled");
  void open(const QString& filename);
  std::vector<AbstractCtrlCutItem*> load(const QString& filename, bool loadVector = true, bool loadRaster = true);
  void reset();
  void detachDocumentHolder();
  void update(const QRectF &rect = QRectF());
 // void keyPressEvent(QKeyEvent *event);

signals:
   void itemMoved(QGraphicsItem *movedItem, const QPointF &movedFromPosition);

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent *event);
   void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
   virtual void drawBackground(QPainter *painter, const QRectF &rect);
private:
  void makeBackground();
  DocumentHolderPtr docHolder;
  QGraphicsPolygonItem* backgroundItem;
  QGraphicsItemGroup *laserbed;
  QGraphicsItem *movingItem;
  QPointF oldPos;
  qreal currentZ;
};

#endif

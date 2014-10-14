/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#ifndef _WIDGETDOCK_H_
#define _WIDGETDOCK_H_

// Workaround for https://bugreports.qt-project.org/browse/QTBUG-22829
#ifdef Q_MOC_RUN
#define BOOST_LEXICAL_CAST_INCLUDED
#define BOOST_TT_HAS_OPERATOR_HPP_INCLUDED
#endif

#include <QDockWidget>
#include "config/EngraveSettings.hpp"
#include "config/CutSettings.hpp"

typedef std::shared_ptr<class Document> DocumentPtr;

class ObjectPropertyWidget : public QWidget {
  Q_OBJECT
public:
  ObjectPropertyWidget(QWidget *parent = NULL);
  ~ObjectPropertyWidget();
  
  void applyLaserConfig(class Document &document);
  void updateLaserConfig(Document &document);

  void updateEngraveProperties(const EngraveSettings::KeyBase&  key);
  void updateCutProperties(const CutSettings::KeyBase&  key);
  void enable(class AbstractCtrlCutItem* item);
  void setDocument(DocumentPtr doc);
  void enableCutItem(class CutItem* ci);
  void enableEngraveItem(class EngraveItem* ei);
  void disable();
  void update();

public slots:
  void on_power_update(const QString &);
  void on_speed_update(const QString &);
  void on_frequency_update(const QString &);
  void on_posX_update(const QString &);
  void on_posY_update(const QString &);
  void on_sort_update(int);
  void on_direction_update(int);
  void on_dithering_update(int);
  void on_unit_update(int);
  void on_autofocus_update(int);
  void on_title_update(const QString&);
  void on_reduce_update(const QString&);


private:
  enum State { NONE, Engraving, Cut};
  State currentState;
  Unit currentUnit;
  uint32_t currentResolution;

  DocumentPtr doc;
  class CutItem* ci;
  class EngraveItem* ei;
};

#endif

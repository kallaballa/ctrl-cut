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
#ifndef LASERDIALOG_H_
#define LASERDIALOG_H_

#include <QDialog>
#include "ui_LaserDialog.h"

class LaserDialog : public QDialog, private Ui::LaserDialog
{
  Q_OBJECT
public:
  LaserDialog(QWidget *parent = NULL);
  ~LaserDialog();
  
  void applyLaserConfig(class Document &document);
  void updateLaserConfig(Document &document);

private slots:
  void on_rasterPowerSlider_valueChanged(int);
  void on_rasterSpeedSlider_valueChanged(int);
  void on_vectorPowerSlider_valueChanged(int);
  void on_vectorSpeedSlider_valueChanged(int);
  void on_vectorFreqSlider_valueChanged(int);
  void on_rasterPower_textEdited(const QString &);
  void on_rasterSpeed_textEdited(const QString &);
  void on_vectorPower_textEdited(const QString &);
  void on_vectorSpeed_textEdited(const QString &);
  void on_vectorFreq_textEdited(const QString &);

private:
};

#endif
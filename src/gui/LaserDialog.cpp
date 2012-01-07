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
#include "LaserDialog.h"
#include "config/EngraveSettings.hpp"
#include "config/CutSettings.hpp"
#include "config/DocumentSettings.hpp"
#include <assert.h>

typedef DocumentSettings DS;
typedef EngraveSettings ES;
typedef CutSettings CS;

LaserDialog::LaserDialog(QWidget *parent) : QDialog(parent)
{
  setupUi(this);

  QIntValidator *percentvalidator = new QIntValidator(0, 100, this);
  this->rasterPower->setValidator(percentvalidator);
  this->rasterSpeed->setValidator(percentvalidator);
  this->vectorPower->setValidator(percentvalidator);
  this->vectorSpeed->setValidator(percentvalidator);

  QIntValidator *freqvalidator = new QIntValidator(0, 5000, this);
  this->vectorFreq->setValidator(freqvalidator);
}

LaserDialog::~LaserDialog()
{
}

void LaserDialog::updateLaserConfig(Document& document)
{
  document.put(DS::AUTO_FOCUS, this->autoFocusBox->isChecked());
  QStringList res = this->resolutionCombo->currentText().split(" ");
  if (res.length() > 0) document.put(DS::RESOLUTION, res[0].toInt());

  document.put(DS::ENABLE_RASTER, !this->vectorRadio->isChecked());
  document.put(DS::ENABLE_VECTOR, !this->rasterRadio->isChecked());
  document.put(ES::ESPEED, this->rasterSpeedSlider->value());
  document.put(ES::EPOWER, this->rasterPowerSlider->value());
  document.put(ES::DIRECTION, this->rasterDirection->currentIndex());
  const QString &dither = this->rasterDithering->currentText();

  ES::Dithering dithering;
  if (dither == "Default") dithering = ES::DEFAULT_DITHERING;
  else if (dither == "Bayer") dithering = ES::BAYER;
  else if (dither == "Floyd-Steinberg") dithering = ES::FLOYD_STEINBERG;
  else if (dither == "Jarvis") dithering = ES::JARVIS;
  else if (dither == "Burke") dithering = ES::BURKE;
  else if (dither == "Stucki") dithering = ES::STUCKI;
  else if (dither == "Sierra2") dithering = ES::SIERRA2;
  else if (dither == "Sierra3") dithering = ES::SIERRA3;

  document.put(ES::DITHERING, dithering);

  const QString& strOptimize = this->cutOptimize->currentText();

  CS::Optimize optimize;
  if (strOptimize == "Simple") optimize = CS::SIMPLE;
  else if (strOptimize == "Inner-Outer") optimize = CS::INNER_OUTER;
  else if (strOptimize == "Shortest Path") optimize = CS::SHORTEST_PATH;

    document.put(CS::OPTIMIZE, optimize);
  document.put(CS::CSPEED, this->vectorSpeedSlider->value());
  document.put(CS::CPOWER, this->vectorPowerSlider->value());
  document.put(CS::FREQUENCY, this->vectorFreqSlider->value());
}

void LaserDialog::applyLaserConfig(Document& document)
{
  this->autoFocusBox->setChecked(document.get(DS::AUTO_FOCUS));
  this->resolutionCombo->setCurrentIndex(this->resolutionCombo->findText(QString::number(document.get(DS::RESOLUTION)) + " DPI"));

  if (document.get(DS::ENABLE_RASTER) && document.get(DS::ENABLE_VECTOR)) this->combinedRadio->setChecked(true);
  else if (document.get(DS::ENABLE_RASTER)) this->rasterRadio->setChecked(true);
  else this->vectorRadio->setChecked(true);

  this->rasterSpeedSlider->setValue(document.get(ES::ESPEED));
  this->rasterPowerSlider->setValue(document.get(ES::EPOWER));
  this->rasterDirection->setCurrentIndex(document.get(ES::DIRECTION));

  QString ditherstr;
  switch (document.get(ES::DITHERING)) {
  case EngraveSettings::BAYER:
    ditherstr = "Bayer";
    break;
  case EngraveSettings::FLOYD_STEINBERG:
    ditherstr = "Floyd-Steinberg";
    break;
  case EngraveSettings::JARVIS:
    ditherstr = "Jarvis";
    break;
  case EngraveSettings::BURKE:
    ditherstr = "Burke";
    break;
  case EngraveSettings::STUCKI:
    ditherstr = "Stucki";
    break;
  case EngraveSettings::SIERRA2:
    ditherstr = "Sierra2";
    break;
  case EngraveSettings::SIERRA3:
    ditherstr = "Sierra3";
    break;
  case EngraveSettings::DEFAULT_DITHERING:
    ditherstr = "Default";
    break;
  default:
    assert(false && "Unhandled raster dithering setting");
  }
  this->rasterDithering->setCurrentIndex(this->rasterDithering->findText(ditherstr));

  QString strOptimize;
  switch (document.get(CS::OPTIMIZE)) {
  case CS::SIMPLE:
    strOptimize = "Simple";
    break;
  case CS::INNER_OUTER:
    strOptimize = "Inner-Outer";
    break;
  case CS::SHORTEST_PATH:
    strOptimize = "Shortest Path";
    break;
  default:
    assert(false && "Unhandled cut optimize setting");
  }
  this->cutOptimize->setCurrentIndex(this->cutOptimize->findText(strOptimize));

  this->vectorSpeedSlider->setValue(document.get(CS::CSPEED));
  this->vectorPowerSlider->setValue(document.get(CS::CPOWER));
  this->vectorFreqSlider->setValue(document.get(CS::FREQUENCY));
}

void LaserDialog::on_rasterPowerSlider_valueChanged(int val)
{
  this->rasterPower->setText(QString::number(val));
}

void LaserDialog::on_rasterSpeedSlider_valueChanged(int val)
{
  this->rasterSpeed->setText(QString::number(val));
}

void LaserDialog::on_vectorPowerSlider_valueChanged(int val)
{
  this->vectorPower->setText(QString::number(val));
}

void LaserDialog::on_vectorSpeedSlider_valueChanged(int val)
{
  this->vectorSpeed->setText(QString::number(val));
}

void LaserDialog::on_rasterPower_textEdited(const QString &text)
{
  this->rasterPowerSlider->setValue(text.toInt());
}

void LaserDialog::on_rasterSpeed_textEdited(const QString &text)
{
  this->rasterSpeedSlider->setValue(text.toInt());
}

void LaserDialog::on_vectorPower_textEdited(const QString &text)
{
  this->vectorPowerSlider->setValue(text.toInt());
}

void LaserDialog::on_vectorSpeed_textEdited(const QString &text)
{
  this->vectorSpeedSlider->setValue(text.toInt());
}

void LaserDialog::on_vectorFreqSlider_valueChanged(int val)
{
  this->vectorFreq->setText(QString::number(val));
}

void LaserDialog::on_vectorFreq_textEdited(const QString &text)
{
  this->vectorFreqSlider->setValue(text.toInt());
}


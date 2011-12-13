#include "LaserDialog.h"
#include "config/EngraveSettings.h"
#include "config/CutSettings.h"
#include "config/DocumentSettings.h"
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
  DS &ds = document.settings;
  ds.put(DS::AUTO_FOCUS, this->autoFocusBox->isChecked());
  QStringList res = this->resolutionCombo->currentText().split(" ");
  if (res.length() > 0) ds.put(DS::RESOLUTION, res[0].toInt());

  ds.put(DS::ENABLE_RASTER, !this->vectorRadio->isChecked());
  ds.put(DS::ENABLE_VECTOR, !this->rasterRadio->isChecked());
  ds.put(ES::ESPEED, this->rasterSpeedSlider->value());
  ds.put(ES::EPOWER, this->rasterPowerSlider->value());
  ds.put(ES::DIRECTION, this->rasterDirection->currentIndex());
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

  ds.put(ES::DITHERING, dithering);
  ds.put(CS::CSPEED, this->vectorSpeedSlider->value());
  ds.put(CS::CPOWER, this->vectorPowerSlider->value());
  ds.put(CS::FREQUENCY, this->vectorFreqSlider->value());
}

void LaserDialog::applyLaserConfig(Document& document)
{
  DS &ds = document.settings;
  this->autoFocusBox->setChecked(ds.get(DS::AUTO_FOCUS));
  this->resolutionCombo->setCurrentIndex(this->resolutionCombo->findText(QString::number(ds.get(DS::RESOLUTION)) + " DPI"));

  if (ds.get(DS::ENABLE_RASTER) && ds.get(DS::ENABLE_VECTOR)) this->combinedRadio->setChecked(true);
  else if (ds.get(DS::ENABLE_RASTER)) this->rasterRadio->setChecked(true);
  else this->vectorRadio->setChecked(true);

  this->rasterSpeedSlider->setValue(ds.get(ES::ESPEED));
  this->rasterPowerSlider->setValue(ds.get(ES::EPOWER));
  this->rasterDirection->setCurrentIndex(ds.get(ES::DIRECTION));

  QString ditherstr;
  switch (ds.get(ES::DITHERING)) {
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

  this->vectorSpeedSlider->setValue(ds.get(CS::CSPEED));
  this->vectorPowerSlider->setValue(ds.get(CS::CPOWER));
  this->vectorFreqSlider->setValue(ds.get(CS::FREQUENCY));
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

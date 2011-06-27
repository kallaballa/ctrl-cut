#include "LaserDialog.h"

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

void LaserDialog::updateLaserConfig(LaserConfig &config)
{
  config.focus = this->autoFocusBox->isChecked();
  QStringList res = this->resolutionCombo->currentText().split(" ");
  if (res.length() > 0) config.resolution = res[0].toInt();

  config.enable_raster = !this->vectorRadio->isChecked();
  config.enable_vector = !this->rasterRadio->isChecked();
  config.raster_speed = this->rasterSpeedSlider->value();
  config.raster_power = this->rasterPowerSlider->value();
  config.raster_direction = (LaserConfig::RasterDirection)this->rasterDirection->currentIndex();

  const QString &dither = this->rasterDithering->currentText();
  if (dither == "Default") config.raster_dithering = LaserConfig::DITHER_DEFAULT;
  else if (dither == "Bayer") config.raster_dithering = LaserConfig::DITHER_BAYER;
  else if (dither == "Floyd-Steinberg") config.raster_dithering = LaserConfig::DITHER_FLOYD_STEINBERG;
  else if (dither == "Jarvis") config.raster_dithering = LaserConfig::DITHER_JARVIS;
  else if (dither == "Burke") config.raster_dithering = LaserConfig::DITHER_BURKE;
  else if (dither == "Stucki") config.raster_dithering = LaserConfig::DITHER_STUCKI;
  else if (dither == "Sierra2") config.raster_dithering = LaserConfig::DITHER_SIERRA2;
  else if (dither == "Sierra3") config.raster_dithering = LaserConfig::DITHER_SIERRA3;

  config.vector_speed = this->vectorSpeedSlider->value();
  config.vector_power = this->vectorPowerSlider->value();
  config.vector_freq = this->vectorFreqSlider->value();
}

void LaserDialog::applyLaserConfig(LaserConfig &config)
{
  this->autoFocusBox->setChecked(config.focus);
  this->resolutionCombo->setCurrentIndex(this->resolutionCombo->findText(QString::number(config.resolution) + " DPI"));

  if (config.enable_raster && config.enable_vector) this->combinedRadio->setChecked(true);
  else if (config.enable_raster) this->rasterRadio->setChecked(true);
  else this->vectorRadio->setChecked(true);

  this->rasterSpeedSlider->setValue(config.raster_speed);
  this->rasterPowerSlider->setValue(config.raster_power);
  this->rasterDirection->setCurrentIndex(config.raster_direction);

  QString ditherstr = "Default";
  switch (config.raster_dithering) {
  case LaserConfig::DITHER_BAYER:
    ditherstr = "Bayer";
    break;
  case LaserConfig::DITHER_FLOYD_STEINBERG:
    ditherstr = "Floyd-Steinberg";
    break;
  case LaserConfig::DITHER_JARVIS:
    ditherstr = "Jarvis";
    break;
  case LaserConfig::DITHER_BURKE:
    ditherstr = "Burke";
    break;
  case LaserConfig::DITHER_STUCKI:
    ditherstr = "Stucki";
    break;
  case LaserConfig::DITHER_SIERRA2:
    ditherstr = "Sierra2";
    break;
  case LaserConfig::DITHER_SIERRA3:
    ditherstr = "Sierra3";
    break;
  }
  this->rasterDithering->setCurrentIndex(this->rasterDithering->findText(ditherstr));

  this->vectorSpeedSlider->setValue(config.vector_speed);
  this->vectorPowerSlider->setValue(config.vector_power);
  this->vectorFreqSlider->setValue(config.vector_freq);
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

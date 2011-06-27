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
  // FIXME: convert from index to value config.resolution = this->resolutionCombo->currentIndex();
  config.enable_raster = !this->vectorRadio->isChecked();
  config.enable_vector = !this->rasterRadio->isChecked();
  config.raster_speed = this->rasterSpeedSlider->value();
  config.raster_power = this->rasterPowerSlider->value();
  //  config.raster_direction = FIXME;
  //  config.raster_dithering = FIXME;

  config.vector_speed = this->vectorSpeedSlider->value();
  config.vector_power = this->vectorPowerSlider->value();
  config.vector_freq = this->vectorFreqSlider->value();
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

/*
 * Ctrl-Cut - Laser cutter toolchain
 * See LICENSE file
 * Copyright (C) 2011 Amir Hassan <amir@viel-zu.org> and Marius kintel <kintel@kintel.net>
 */
#include "PreviewDialog.hpp"
#include <QString>
#include <assert.h>
#include "svg/SvgWriter.hpp"
#include <fstream>
#include <boost/filesystem.hpp>

PreviewDialog::PreviewDialog(Document& doc, QWidget *parent) : QDialog(parent)
{
  setupUi(this);

  progressDialog.setWindowFlags(Qt::Dialog | Qt::Desktop);
  progressDialog.setWindowModality(Qt::ApplicationModal);
  QObject::connect(&this->progressDialog, SIGNAL(canceled()), this, SLOT(on_cancel()));
  progressDialog.show();
  progressDialog.setValue(0);

  Document copy = doc;
  copy.mergeCuts();
  progressDialog.setValue(10);
  copy.optimize();
  progressDialog.setValue(90);

  string filename = (boost::filesystem::temp_directory_path() /= boost::filesystem::unique_path()).native() + ".svg";
  std::ofstream os(filename);
  SvgWriter svg(copy, os);
  svg.write(copy);
  os.flush();
  progressDialog.setValue(100);
  progressDialog.hide();

  this->webView->setUrl(QUrl::fromLocalFile(QString::fromStdString(filename)));
  this->webView->setZoomFactor(0.05);
  this->zoomBox->setCurrentIndex(10);


  QObject::connect(this->zoomBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(on_zoom_triggered(const QString&)));
}

PreviewDialog::~PreviewDialog() {
}

void PreviewDialog::on_zoom_triggered(const QString& s) {
  this->webView->setZoomFactor(s.toDouble()/100.0);
}

void PreviewDialog::on_cancel() {
}

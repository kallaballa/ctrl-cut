#include "PreviewDialog.hpp"
#include <QString>
#include <assert.h>
#include "svg/SvgWriter.hpp"
#include <sstream>

PreviewDialog::PreviewDialog(Document& doc, QWidget *parent) : QDialog(parent)
{
  setupUi(this);

  typedef DocumentSettings DS;
  Document copy = doc;
  copy.optimize();
  std::stringstream ss;
  SvgWriter svg(copy.get(DS::WIDTH).in(PX), copy.get(DS::HEIGHT).in(PX), copy.get(DS::RESOLUTION), copy.get(DS::TITLE), ss);
  svg.write(copy);

  this->webView->setHtml(QString::fromStdString(ss.str()));
  this->webView->setZoomFactor(0.05);
  this->zoomBox->setCurrentIndex(10);
  connect(this->zoomBox, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(on_zoom_triggered(const QString&)));
}

PreviewDialog::~PreviewDialog() {
}

void PreviewDialog::on_zoom_triggered(const QString& s) {
  this->webView->setZoomFactor(s.toDouble()/100.0);
}

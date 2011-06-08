#include "MainWindow.h"
#include <QtGui>

#include "FileParser.h"
#include "vector/Cut.h"
#include "vector/Polyline.h"
#include "vector/Edge.h"
#include "vector/Vertex.h"

#include <assert.h>

MainWindow::MainWindow()
{
  setupUi(this);

}

MainWindow::~MainWindow()
{
}

void MainWindow::on_fileOpenAction_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this, "Open File", "", "Supported files (*.ps *.vector)");
  if (!filename.isEmpty()) {
    Cut *cut = NULL;


    QFileInfo finfo(filename);
    QString suffix = finfo.suffix();
    if (suffix == "ps") {
      PostscriptParser *psparser = new PostscriptParser(LaserConfig::inst());
      switch (LaserConfig::inst().raster_dithering) {
      case LaserConfig::DITHER_DEFAULT:
        psparser->setRasterFormat(PostscriptParser::BITMAP);
        break;
      case LaserConfig::DITHER_THRESHOLD:
      case LaserConfig::DITHER_BAYER:
      case LaserConfig::DITHER_FLOYD_STEINBERG:
      case LaserConfig::DITHER_FLOYD_JARVIS:
      case LaserConfig::DITHER_FLOYD_BURKE:
      case LaserConfig::DITHER_FLOYD_STUCKI:
      case LaserConfig::DITHER_FLOYD_SIERRA2:
      case LaserConfig::DITHER_FLOYD_SIERRA3:
        psparser->setRasterFormat(PostscriptParser::GRAYSCALE);
        break;
        
      default:
        assert(false);
      }
      cups_file_t *input_file = cupsFileOpen(filename.toLocal8Bit(), "r");
      if (!input_file) {
        LOG_ERR("Error opening postscript file");
        return;
      }
      LaserConfig::inst().basename = finfo.baseName().toStdString();
      if (!psparser->parse(input_file)) {
        LOG_FATAL("Error processing postscript");
        return;
      }

      cut = Cut::load(psparser->getVectorData());
      if (!cut) {
        fprintf(stderr, "Error: Unable to open postscript file\n");
        return;
      }
    }
    else if (suffix == "vector") {
      cut = Cut::load(filename.toStdString());
      if (!cut) {
        fprintf(stderr, "Error: Unable to open vector file\n");
        return;
      }
    }

    if (cut) {
      const Mesh &mesh = cut->getMesh();
      for (Mesh::const_iterator iter = mesh.begin(); iter != mesh.end(); iter++) {
        const Edge &edge = **iter;
        this->graphicsView->scene()->addLine(edge[0][0], edge[0][1], edge[1][0], edge[1][1]);
      }
    }
  }
}

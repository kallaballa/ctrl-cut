#include "MainWindow.h"
#include <QtGui>

#include "FileParser.h"
#include "LaserJob.h"
#include "Driver.h"
#include "vector/CutModel.h"
#include "vector/Geometry.h"

#include "LpdClient.h"
#include "StreamUtils.h"
#include "GroupItem.h"
#include "CtrlCutScene.h"

#include <assert.h>

MainWindow::MainWindow() : cutmodel(NULL)
{
  this->lpdclient = new LpdClient(this);
  this->lpdclient->setObjectName("lpdclient");

  setupUi(this);

  this->scene = new CtrlCutScene(this);
  this->graphicsView->setScene(this->scene);

  connect(this->scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()));
  connect(this->scene, SIGNAL(sceneRectChanged(const QRectF&)),
          this->graphicsView, SLOT(updateSceneRect(const QRectF&)));
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_fileOpenAction_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this, "Open File", "", "Supported files (*.ps *.vector)");
  if (!filename.isEmpty()) {
    if (this->cutmodel) {
      delete this->cutmodel;
      this->cutmodel = NULL;
    }

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

      this->cutmodel = CutModel::load(psparser->getVectorData());
      if (!this->cutmodel) {
        fprintf(stderr, "Error: Unable to open postscript file\n");
        return;
      }
    }
    else if (suffix == "vector") {
      this->cutmodel = CutModel::load(filename.toStdString());
      if (!this->cutmodel) {
        fprintf(stderr, "Error: Unable to open vector file\n");
        return;
      }
    }

    if (this->cutmodel) {
      this->firstitem = NULL;
      QGraphicsItemGroup *parentitem = new QGraphicsItemGroup();
      //QGraphicsItemGroup *parentitem = new GroupItem();
      parentitem->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
      for (CutModel::SegmentIter iter = this->cutmodel->beginSegments(); iter != this->cutmodel->endSegments(); iter++) {
        const Segment &segment = **iter;
        QGraphicsLineItem *line = 
          // new QGraphicsLineItem(segment[0][0], segment[0][1], segment[1][0], segment[1][1]);
          new QGraphicsLineItem(segment[0][0], segment[0][1], segment[1][0], segment[1][1],
                                parentitem);
        //        line->setFlags(QGraphicsItem::ItemIsSelectable);
        //        this->graphicsView->scene()->addItem(line);
        parentitem->addToGroup(line);
        if (!this->firstitem) this->firstitem = line;
      }
      this->scene->addItem(parentitem);
    }
  }
}

#if 0
void MainWindow::on_filePrintAction_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this, "Open RTL File", "", "Supported files (*.prn *.raw)");
  if (!filename.isEmpty()) {
    printf("RTL file: %s\n", filename.toLocal8Bit().data());

    QFile rtlfile(filename);
    if (!rtlfile.open(QIODevice::ReadOnly)) {
      fprintf(stderr, "Unable to open RTL file\n");
      return;
    }

    this->lpdclient->print("MyDocument", rtlfile.readAll());
    rtlfile.close();

  }
}
#endif

void MainWindow::on_filePrintAction_triggered()
{
  if (!this->cutmodel) {
    fprintf(stderr, "No model loaded\n");
    return;
  }

  QStringList items;
  items << "Lazzzor" << "localhost";
  bool ok;
  QString item = QInputDialog::getItem(this, "Send to where?", "Send to where?", items, 0, false, &ok);
  if (ok && !item.isEmpty()) {
    QString host = (item == "Lazzzor")?"10.20.30.27":"localhost";

    LaserJob job(&LaserConfig::inst(), "kintel", "jobname", "jobtitle");
    job.addCut(this->cutmodel);
    Driver drv;
    QByteArray rtlbuffer;
    ByteArrayOStreambuf streambuf(rtlbuffer);
    std::ostream ostream(&streambuf);
    drv.process(&job, ostream);
    
    this->lpdclient->print(host, "MyDocument", rtlbuffer);
  }
}

void MainWindow::on_lpdclient_done(bool error)
{
  if (error) fprintf(stderr, "LPD error\n");
  else printf("LPD done\n");
}

void MainWindow::on_lpdclient_progress(int done, int total)
{
  printf("Progress: %.0f%%\n", 100.0f*done/total);
}

void MainWindow::sceneSelectionChanged()
{
  printf("selectionChanged\n");
  foreach (QGraphicsItem *item, this->scene->selectedItems()) {
    printf("item: %p\n", item);
  }
}

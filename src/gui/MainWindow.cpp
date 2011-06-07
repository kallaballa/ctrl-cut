#include "MainWindow.h"
#include <QtGui>
#include "vector/Cut.h"
#include "vector/Polyline.h"
#include "vector/Edge.h"
#include "vector/Vertex.h"

MainWindow::MainWindow()
{
  setupUi(this);

  this->scene = new QGraphicsScene();
  this->graphicsView->setScene(this->scene);
}

MainWindow::~MainWindow()
{
}

void MainWindow::on_fileOpenAction_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this, "Open File", "", "Vector files (*.vector)");
  if (!filename.isEmpty()) {
    Cut *cut = Cut::load(filename.toStdString());
    if (!cut) {
      fprintf(stderr, "Error: Unable to open vector file\n");
      return;
    }
    const Mesh &mesh = cut->getMesh();
    for (Mesh::const_iterator iter = mesh.begin(); iter != mesh.end(); iter++) {
      const Edge &edge = **iter;
      this->scene->addLine(edge[0][0], edge[0][1], edge[1][0], edge[1][1]);
    }
  }
}

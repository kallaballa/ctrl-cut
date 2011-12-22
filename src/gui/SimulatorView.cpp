#include "SimulatorView.h"
#include <QDragEnterEvent>
#include <QUrl>
#include <QDebug>
 
SimulatorView::SimulatorView(QWidget* parent) : NavigationView(parent)
{
}

void SimulatorView::dragEnterEvent(QDragEnterEvent *event)
{
  event->accept();
}

void SimulatorView::dragMoveEvent(QDragMoveEvent *event)
{
  event->accept();
}

void SimulatorView::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}

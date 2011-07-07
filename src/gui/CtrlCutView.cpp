#include "CtrlCutView.h"
#include <QDragEnterEvent>
#include <QUrl>
#include <QDebug>
 
CtrlCutView::CtrlCutView(QWidget* parent) : NavigationView(parent)
{
}

void CtrlCutView::dragEnterEvent(QDragEnterEvent *event)
{
  event->setDropAction(Qt::CopyAction);
  event->accept();
}

void CtrlCutView::dragMoveEvent(QDragMoveEvent *event)
{
  event->setDropAction(Qt::CopyAction);
  event->accept();
}

void CtrlCutView::dropEvent(QDropEvent *event)
{
  const QMimeData *mimeData = event->mimeData();

  // FIXME: Only accept supported file types
  if (mimeData->hasUrls()) {
    QList<QUrl> urlList = mimeData->urls();
    QString text;
    for (int i = 0; i < urlList.size() && i < 32; ++i) {
      emit fileDropped(urlList.at(i).path());
    }
  }
  
  event->acceptProposedAction();
}

void CtrlCutView::dragLeaveEvent(QDragLeaveEvent *event)
{
  event->accept();
}

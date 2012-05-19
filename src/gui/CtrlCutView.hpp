#ifndef CTRLCUTVIEW_H_
#define CTRLCUTVIEW_H_
 
#include "NavigationView.hpp"
 
class CtrlCutView : public NavigationView
{
  Q_OBJECT
public:
  CtrlCutView(QWidget* parent = NULL);

signals:
  void fileDropped(const QString &filename);

private:
  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);
  void dropEvent(QDropEvent *event);
};
 
#endif

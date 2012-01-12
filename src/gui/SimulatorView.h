#ifndef SIMULATORVIEW_H_
#define SIMULATORVIEW_H_
 
#include "NavigationView.h"
 
class SimulatorView : public NavigationView
{
  Q_OBJECT
public:
  SimulatorView(QWidget* parent = NULL);
private:
  void dragEnterEvent(QDragEnterEvent *event);
  void dragMoveEvent(QDragMoveEvent *event);
  void dragLeaveEvent(QDragLeaveEvent *event);
};
 
#endif

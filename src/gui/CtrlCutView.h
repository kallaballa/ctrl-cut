#ifndef CTRLCUTVIEW_H_
#define CTRLCUTVIEW_H_
 
#include <QGraphicsView>
#include <QGraphicsRectItem>
 
class CtrlCutView : public QGraphicsView
{
  Q_OBJECT;
public:
  CtrlCutView(QWidget* parent = NULL);
  
private:
  void scaleView(qreal scaleFactor);
  
  void keyPressEvent(QKeyEvent *event);
  void wheelEvent(QWheelEvent* event);
  void mouseDoubleClickEvent(QMouseEvent *event);
};
 
#endif

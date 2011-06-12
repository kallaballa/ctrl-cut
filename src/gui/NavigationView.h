#ifndef MYGRAPHICSVIEW_H_
#define MYGRAPHICSVIEW_H_
 
#include <QGraphicsView>
#include <QGraphicsRectItem>
 
class NavigationView : public QGraphicsView
{
  Q_OBJECT;
public:
  NavigationView(QWidget* parent = NULL);

signals:
  void matrixUpdated();

private:
  void scaleView(qreal scaleFactor, QPointF center);
  
  void keyPressEvent(QKeyEvent *event);
  void wheelEvent(QWheelEvent* event);
  void mouseDoubleClickEvent(QMouseEvent *event);
  void paintEvent(QPaintEvent *event);
};
 
#endif

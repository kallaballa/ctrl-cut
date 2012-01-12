#ifndef NAVIGATIONVIEW_H_
#define NAVIGATIONVIEW_H_
 
#include <QGraphicsView>
#include <QGraphicsRectItem>
 
class NavigationView : public QGraphicsView
{
  Q_OBJECT
public:
  NavigationView(QWidget* parent = NULL);

public slots:
  void updateSceneRect(const QRectF &);

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

#ifndef SIMULATORSCENE_H_
#define SIMULATORSCENE_H_

#include <QGraphicsScene>

class SimulatorScene : public QGraphicsScene
{
  Q_OBJECT;
public:
  SimulatorScene(QObject *parent = 0);
  ~SimulatorScene() {}

  void keyPressEvent(QKeyEvent *event);
  void reset();
};

#endif

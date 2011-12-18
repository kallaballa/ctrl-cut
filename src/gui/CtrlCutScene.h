#ifndef CTRLCUTSCENE_H_
#define CTRLCUTSCENE_H_

#include <QGraphicsScene>

class CtrlCutScene : public QGraphicsScene
{
  Q_OBJECT;
public:
  CtrlCutScene(QObject *parent = 0);
  ~CtrlCutScene() {}

  void keyPressEvent(QKeyEvent *event);
};

#endif

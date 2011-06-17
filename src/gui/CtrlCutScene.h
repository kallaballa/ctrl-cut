#ifndef CTRLCUTSCENE_H_
#define CTRLCUTSCENE_H_

#include <QGraphicsScene>

class CtrlCutScene : public QGraphicsScene
{
public:
  CtrlCutScene(QObject *parent = 0);
  ~CtrlCutScene() {}

  void keyPressEvent(QKeyEvent *event);

};

#endif

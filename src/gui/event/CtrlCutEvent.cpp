#include "CtrlCutEvent.h"

CtrlCutEvent::ActionMap CtrlCutEvent::actionMap;

void CtrlCutEvent::registerAction(int mod, int key, CtrlCutEvent::Action  action) {
  actionMap[KeyCombination(mod, key)] = action;
}

void CtrlCutEvent::registerAction(int key, CtrlCutEvent::Action action) {
  actionMap[KeyCombination(0, key)] = action;
}

bool CtrlCutEvent::act(QKeyEvent& event, QList<QGraphicsItem *> items) {
  ActionMap::iterator it =  actionMap.find(KeyCombination(event.modifiers(), event.key()));
  if(it != actionMap.end()) {
    (*it).second(items);
    return true;
  }
  return false;
}

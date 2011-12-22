#ifndef CTRLCUTEVENT_H_
#define CTRLCUTEVENT_H_

#include <assert.h>
#include <stdint.h>
#include <map>
#include <qlist.h>
#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qevent.h>
#include <qnamespace.h>
#include <functional>
#include <boost/function.hpp>

class CtrlCutEvent {
public:
  class KeyCombination {
  public:
    KeyCombination() {}
    KeyCombination(int mod, int key) : mod(mod), key(key) {}
    bool operator<(const KeyCombination &right) const {
      return (mod < right.mod || (mod == right.mod && key < right.key));
    }

    int mod;
    int key;
  };

  typedef boost::function<void (QList<QGraphicsItem *>)> Action;

  typedef std::map<KeyCombination, Action > ActionMap;

  Action NULL_ACTION;

  struct GroupItems {
    QGraphicsScene& scene;
    GroupItems(QGraphicsScene& scene): scene(scene) {}
    virtual void operator()(QList<QGraphicsItem *> items) const {
      QGraphicsItemGroup* group = this->scene.createItemGroup(items);
      group->setFlags(QGraphicsItemGroup::ItemIsSelectable | QGraphicsItemGroup::ItemIsMovable);
    }
  };

  struct UngroupItems  {
    QGraphicsScene& scene;
    UngroupItems(QGraphicsScene& scene): scene(scene) {}
    virtual void operator()(QList<QGraphicsItem *> items) const {
      QGraphicsItemGroup* group;
      foreach (QGraphicsItem *item, items) {
        if((group = dynamic_cast<QGraphicsItemGroup* >(item))) {
          this->scene.destroyItemGroup(group);
        }
      }
    }
  };

  struct MoveItems{
    QPointF delta;

    MoveItems(QPointF delta = QPointF()) : delta(delta) {}

    void operator()(QList<QGraphicsItem *>& items, QPointF to = QPointF()) const {
      foreach (QGraphicsItem *item, items) {
        QPointF pos = item->pos();
        assert((this->delta.x() == 0 && this->delta.y() == 0) || (to.x() == 0 && to.y() == 0));
        if(to.x() == 0 && to.y() == 0)
          pos += delta;
        else
          pos = to;
        item->setPos(pos);
      }
    }

    static MoveItems UP(qreal step) {
      return MoveItems(QPointF(0, -step));
    }

    static MoveItems DOWN(qreal step) {
      return MoveItems(QPointF(0, step));
    }

    static MoveItems LEFT(qreal step) {
      return MoveItems(QPointF(-step,0));
    }

    static MoveItems RIGHT(qreal step) {
      return MoveItems(QPointF(step,0));
    }
  };

  CtrlCutEvent(){};
  virtual ~CtrlCutEvent(){};

  static void registerAction(int mod, int key, Action  action);
  static void registerAction(int key, Action action);
  static bool act(QKeyEvent& event, QList<QGraphicsItem *> items);
private:
  static ActionMap actionMap;
};

#endif /* CTRLCUTEVENT_H_ */

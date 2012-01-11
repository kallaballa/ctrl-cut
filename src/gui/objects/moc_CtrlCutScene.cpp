/****************************************************************************
** Meta object code from reading C++ file 'CtrlCutScene.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../CtrlCutScene.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CtrlCutScene.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CtrlCutScene[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      42,   14,   13,   13, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_CtrlCutScene[] = {
    "CtrlCutScene\0\0movedItem,movedFromPosition\0"
    "itemMoved(QGraphicsItem*,QPointF)\0"
};

const QMetaObject CtrlCutScene::staticMetaObject = {
    { &QGraphicsScene::staticMetaObject, qt_meta_stringdata_CtrlCutScene,
      qt_meta_data_CtrlCutScene, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CtrlCutScene::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CtrlCutScene::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CtrlCutScene::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CtrlCutScene))
        return static_cast<void*>(const_cast< CtrlCutScene*>(this));
    return QGraphicsScene::qt_metacast(_clname);
}

int CtrlCutScene::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGraphicsScene::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: itemMoved((*reinterpret_cast< QGraphicsItem*(*)>(_a[1])),(*reinterpret_cast< const QPointF(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void CtrlCutScene::itemMoved(QGraphicsItem * _t1, const QPointF & _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE

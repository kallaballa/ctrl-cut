/****************************************************************************
** Meta object code from reading C++ file 'MainWindow.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../MainWindow.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MainWindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      43,   11,   11,   11, 0x0a,
      73,   11,   11,   11, 0x0a,
     105,   11,   11,   11, 0x0a,
     136,   11,   11,   11, 0x0a,
     181,  175,   11,   11, 0x0a,
     216,  205,   11,   11, 0x0a,
     247,   11,   11,   11, 0x0a,
     300,  277,   11,   11, 0x0a,
     337,   11,   11,   11, 0x0a,
     353,   11,   11,   11, 0x0a,
     386,  377,   11,   11, 0x0a,
     404,  377,   11,   11, 0x0a,
     424,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0on_helpAboutAction_triggered()\0"
    "on_fileOpenAction_triggered()\0"
    "on_fileImportAction_triggered()\0"
    "on_filePrintAction_triggered()\0"
    "on_toolsMoveToOriginAction_triggered()\0"
    "error\0on_lpdclient_done(bool)\0done,total\0"
    "on_lpdclient_progress(int,int)\0"
    "on_simulateAction_triggered()\0"
    "item,moveStartPosition\0"
    "on_itemMoved(QGraphicsItem*,QPointF)\0"
    "on_deleteItem()\0sceneSelectionChanged()\0"
    "filename\0openFile(QString)\0"
    "importFile(QString)\0simulate()\0"
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_helpAboutAction_triggered(); break;
        case 1: on_fileOpenAction_triggered(); break;
        case 2: on_fileImportAction_triggered(); break;
        case 3: on_filePrintAction_triggered(); break;
        case 4: on_toolsMoveToOriginAction_triggered(); break;
        case 5: on_lpdclient_done((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: on_lpdclient_progress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 7: on_simulateAction_triggered(); break;
        case 8: on_itemMoved((*reinterpret_cast< QGraphicsItem*(*)>(_a[1])),(*reinterpret_cast< const QPointF(*)>(_a[2]))); break;
        case 9: on_deleteItem(); break;
        case 10: sceneSelectionChanged(); break;
        case 11: openFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: importFile((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 13: simulate(); break;
        default: ;
        }
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

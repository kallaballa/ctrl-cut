/****************************************************************************
** Meta object code from reading C++ file 'SimulatorDialog.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SimulatorDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SimulatorDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SimulatorDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x0a,
      41,   16,   16,   16, 0x0a,
      64,   52,   16,   16, 0x0a,
     114,   52,   16,   16, 0x0a,
     173,  161,   16,   16, 0x0a,
     207,  161,   16,   16, 0x0a,
     240,   16,   16,   16, 0x0a,
     263,  249,   16,   16, 0x0a,
     309,  300,   16,   16, 0x2a,
     333,  249,   16,   16, 0x0a,
     370,  300,   16,   16, 0x2a,

       0        // eod
};

static const char qt_meta_stringdata_SimulatorDialog[] = {
    "SimulatorDialog\0\0sceneSelectionChanged()\0"
    "simulate()\0x0,y0,r,g,b\0"
    "engravePixel(coord,coord,uint8_t,uint8_t,uint8_t)\0"
    "drawPixel(coord,coord,uint8_t,uint8_t,uint8_t)\0"
    "x0,y0,x1,y1\0drawMove(coord,coord,coord,coord)\0"
    "drawCut(coord,coord,coord,coord)\0"
    "update()\0filename,clip\0"
    "dumpVectorImage(string,BoundingBox*)\0"
    "filename\0dumpVectorImage(string)\0"
    "dumpRasterImage(string,BoundingBox*)\0"
    "dumpRasterImage(string)\0"
};

const QMetaObject SimulatorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SimulatorDialog,
      qt_meta_data_SimulatorDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SimulatorDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SimulatorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SimulatorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SimulatorDialog))
        return static_cast<void*>(const_cast< SimulatorDialog*>(this));
    if (!strcmp(_clname, "Canvas"))
        return static_cast< Canvas*>(const_cast< SimulatorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int SimulatorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sceneSelectionChanged(); break;
        case 1: simulate(); break;
        case 2: engravePixel((*reinterpret_cast< coord(*)>(_a[1])),(*reinterpret_cast< coord(*)>(_a[2])),(*reinterpret_cast< uint8_t(*)>(_a[3])),(*reinterpret_cast< uint8_t(*)>(_a[4])),(*reinterpret_cast< uint8_t(*)>(_a[5]))); break;
        case 3: drawPixel((*reinterpret_cast< coord(*)>(_a[1])),(*reinterpret_cast< coord(*)>(_a[2])),(*reinterpret_cast< uint8_t(*)>(_a[3])),(*reinterpret_cast< uint8_t(*)>(_a[4])),(*reinterpret_cast< uint8_t(*)>(_a[5]))); break;
        case 4: drawMove((*reinterpret_cast< coord(*)>(_a[1])),(*reinterpret_cast< coord(*)>(_a[2])),(*reinterpret_cast< coord(*)>(_a[3])),(*reinterpret_cast< coord(*)>(_a[4]))); break;
        case 5: drawCut((*reinterpret_cast< coord(*)>(_a[1])),(*reinterpret_cast< coord(*)>(_a[2])),(*reinterpret_cast< coord(*)>(_a[3])),(*reinterpret_cast< coord(*)>(_a[4]))); break;
        case 6: update(); break;
        case 7: dumpVectorImage((*reinterpret_cast< const string(*)>(_a[1])),(*reinterpret_cast< BoundingBox*(*)>(_a[2]))); break;
        case 8: dumpVectorImage((*reinterpret_cast< const string(*)>(_a[1]))); break;
        case 9: dumpRasterImage((*reinterpret_cast< const string(*)>(_a[1])),(*reinterpret_cast< BoundingBox*(*)>(_a[2]))); break;
        case 10: dumpRasterImage((*reinterpret_cast< const string(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

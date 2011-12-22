/****************************************************************************
** Meta object code from reading C++ file 'LaserDialog.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../LaserDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LaserDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LaserDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      52,   12,   12,   12, 0x08,
      91,   12,   12,   12, 0x08,
     130,   12,   12,   12, 0x08,
     169,   12,   12,   12, 0x08,
     207,   12,   12,   12, 0x08,
     242,   12,   12,   12, 0x08,
     277,   12,   12,   12, 0x08,
     312,   12,   12,   12, 0x08,
     347,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LaserDialog[] = {
    "LaserDialog\0\0on_rasterPowerSlider_valueChanged(int)\0"
    "on_rasterSpeedSlider_valueChanged(int)\0"
    "on_vectorPowerSlider_valueChanged(int)\0"
    "on_vectorSpeedSlider_valueChanged(int)\0"
    "on_vectorFreqSlider_valueChanged(int)\0"
    "on_rasterPower_textEdited(QString)\0"
    "on_rasterSpeed_textEdited(QString)\0"
    "on_vectorPower_textEdited(QString)\0"
    "on_vectorSpeed_textEdited(QString)\0"
    "on_vectorFreq_textEdited(QString)\0"
};

const QMetaObject LaserDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_LaserDialog,
      qt_meta_data_LaserDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LaserDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LaserDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LaserDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LaserDialog))
        return static_cast<void*>(const_cast< LaserDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int LaserDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_rasterPowerSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: on_rasterSpeedSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: on_vectorPowerSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: on_vectorSpeedSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: on_vectorFreqSlider_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: on_rasterPower_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: on_rasterSpeed_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 7: on_vectorPower_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 8: on_vectorSpeed_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 9: on_vectorFreq_textEdited((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE

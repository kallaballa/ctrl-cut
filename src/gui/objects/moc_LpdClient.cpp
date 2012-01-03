/****************************************************************************
** Meta object code from reading C++ file 'LpdClient.h'
**
** Created:
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.4)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../LpdClient.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'LpdClient.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.4. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_LpdClient[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      17,   11,   10,   10, 0x05,
      39,   28,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      57,   10,   10,   10, 0x08,
      79,   10,   10,   10, 0x08,
     104,   10,   10,   10, 0x08,
     150,   10,   10,   10, 0x08,
     178,  172,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_LpdClient[] = {
    "LpdClient\0\0error\0done(bool)\0done,total\0"
    "progress(int,int)\0on_socket_connected()\0"
    "on_socket_disconnected()\0"
    "on_socket_error(QAbstractSocket::SocketError)\0"
    "on_socket_readyRead()\0bytes\0"
    "on_socket_bytesWritten(qint64)\0"
};

const QMetaObject LpdClient::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_LpdClient,
      qt_meta_data_LpdClient, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &LpdClient::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *LpdClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *LpdClient::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_LpdClient))
        return static_cast<void*>(const_cast< LpdClient*>(this));
    return QObject::qt_metacast(_clname);
}

int LpdClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: done((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 1: progress((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 2: on_socket_connected(); break;
        case 3: on_socket_disconnected(); break;
        case 4: on_socket_error((*reinterpret_cast< QAbstractSocket::SocketError(*)>(_a[1]))); break;
        case 5: on_socket_readyRead(); break;
        case 6: on_socket_bytesWritten((*reinterpret_cast< qint64(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void LpdClient::done(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void LpdClient::progress(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE

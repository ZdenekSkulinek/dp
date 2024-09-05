/****************************************************************************
** Meta object code from reading C++ file 'capplication.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dppp/Source/SoundAnalyzer/capplication.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'capplication.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CApplication_t {
    QByteArrayData data[30];
    char stringdata0[347];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CApplication_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CApplication_t qt_meta_stringdata_CApplication = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CApplication"
QT_MOC_LITERAL(1, 13, 16), // "isRunningChanged"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 15), // "isStereoChanged"
QT_MOC_LITERAL(4, 47, 15), // "isPausedChanged"
QT_MOC_LITERAL(5, 63, 18), // "frequenciesChanged"
QT_MOC_LITERAL(6, 82, 18), // "displayErrorSignal"
QT_MOC_LITERAL(7, 101, 7), // "caption"
QT_MOC_LITERAL(8, 109, 11), // "description"
QT_MOC_LITERAL(9, 121, 4), // "quit"
QT_MOC_LITERAL(10, 126, 17), // "displayInfoSignal"
QT_MOC_LITERAL(11, 144, 19), // "CAppDisplayInfoEnum"
QT_MOC_LITERAL(12, 164, 4), // "type"
QT_MOC_LITERAL(13, 169, 10), // "stopSignal"
QT_MOC_LITERAL(14, 180, 20), // "displayErrorMyThread"
QT_MOC_LITERAL(15, 201, 19), // "displayInfoMyThread"
QT_MOC_LITERAL(16, 221, 12), // "stopMyThread"
QT_MOC_LITERAL(17, 234, 4), // "init"
QT_MOC_LITERAL(18, 239, 5), // "start"
QT_MOC_LITERAL(19, 245, 4), // "stop"
QT_MOC_LITERAL(20, 250, 5), // "pause"
QT_MOC_LITERAL(21, 256, 7), // "setMode"
QT_MOC_LITERAL(22, 264, 4), // "mode"
QT_MOC_LITERAL(23, 269, 17), // "updateDeviceLists"
QT_MOC_LITERAL(24, 287, 14), // "processMatFile"
QT_MOC_LITERAL(25, 302, 4), // "path"
QT_MOC_LITERAL(26, 307, 9), // "isRunning"
QT_MOC_LITERAL(27, 317, 8), // "isStereo"
QT_MOC_LITERAL(28, 326, 8), // "isPaused"
QT_MOC_LITERAL(29, 335, 11) // "frequencies"

    },
    "CApplication\0isRunningChanged\0\0"
    "isStereoChanged\0isPausedChanged\0"
    "frequenciesChanged\0displayErrorSignal\0"
    "caption\0description\0quit\0displayInfoSignal\0"
    "CAppDisplayInfoEnum\0type\0stopSignal\0"
    "displayErrorMyThread\0displayInfoMyThread\0"
    "stopMyThread\0init\0start\0stop\0pause\0"
    "setMode\0mode\0updateDeviceLists\0"
    "processMatFile\0path\0isRunning\0isStereo\0"
    "isPaused\0frequencies"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CApplication[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
      19,   14, // methods
       4,  164, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       7,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,  109,    2, 0x06 /* Public */,
       3,    0,  110,    2, 0x06 /* Public */,
       4,    0,  111,    2, 0x06 /* Public */,
       5,    0,  112,    2, 0x06 /* Public */,
       6,    3,  113,    2, 0x06 /* Public */,
      10,    3,  120,    2, 0x06 /* Public */,
      13,    0,  127,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
      14,    3,  128,    2, 0x0a /* Public */,
      14,    2,  135,    2, 0x2a /* Public | MethodCloned */,
      15,    3,  140,    2, 0x0a /* Public */,
      15,    2,  147,    2, 0x2a /* Public | MethodCloned */,
      16,    0,  152,    2, 0x0a /* Public */,

 // methods: name, argc, parameters, tag, flags
      17,    0,  153,    2, 0x02 /* Public */,
      18,    0,  154,    2, 0x02 /* Public */,
      19,    0,  155,    2, 0x02 /* Public */,
      20,    0,  156,    2, 0x02 /* Public */,
      21,    1,  157,    2, 0x02 /* Public */,
      23,    0,  160,    2, 0x02 /* Public */,
      24,    1,  161,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Bool,    7,    8,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, 0x80000000 | 11,    7,    8,   12,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void, QMetaType::QString, QMetaType::QString, QMetaType::Bool,    7,    8,    9,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,    8,
    QMetaType::Void, QMetaType::QString, QMetaType::QString, 0x80000000 | 11,    7,    8,   12,
    QMetaType::Void, QMetaType::QString, QMetaType::QString,    7,    8,
    QMetaType::Void,

 // methods: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Int,   22,
    QMetaType::Void,
    QMetaType::Void, QMetaType::QString,   25,

 // properties: name, type, flags
      26, QMetaType::Bool, 0x00495001,
      27, QMetaType::Bool, 0x00495001,
      28, QMetaType::Bool, 0x00495001,
      29, QMetaType::Int, 0x00495001,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,

       0        // eod
};

void CApplication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CApplication *_t = static_cast<CApplication *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->isRunningChanged(); break;
        case 1: _t->isStereoChanged(); break;
        case 2: _t->isPausedChanged(); break;
        case 3: _t->frequenciesChanged(); break;
        case 4: _t->displayErrorSignal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 5: _t->displayInfoSignal((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< CAppDisplayInfoEnum(*)>(_a[3]))); break;
        case 6: _t->stopSignal(); break;
        case 7: _t->displayErrorMyThread((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3]))); break;
        case 8: _t->displayErrorMyThread((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 9: _t->displayInfoMyThread((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< CAppDisplayInfoEnum(*)>(_a[3]))); break;
        case 10: _t->displayInfoMyThread((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2]))); break;
        case 11: _t->stopMyThread(); break;
        case 12: _t->init(); break;
        case 13: _t->start(); break;
        case 14: _t->stop(); break;
        case 15: _t->pause(); break;
        case 16: _t->setMode((*reinterpret_cast< qint32(*)>(_a[1]))); break;
        case 17: _t->updateDeviceLists(); break;
        case 18: _t->processMatFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CApplication::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CApplication::isRunningChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (CApplication::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CApplication::isStereoChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (CApplication::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CApplication::isPausedChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (CApplication::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CApplication::frequenciesChanged)) {
                *result = 3;
            }
        }
        {
            typedef void (CApplication::*_t)(QString , QString , bool );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CApplication::displayErrorSignal)) {
                *result = 4;
            }
        }
        {
            typedef void (CApplication::*_t)(QString , QString , CAppDisplayInfoEnum );
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CApplication::displayInfoSignal)) {
                *result = 5;
            }
        }
        {
            typedef void (CApplication::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CApplication::stopSignal)) {
                *result = 6;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        CApplication *_t = static_cast<CApplication *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< bool*>(_v) = _t->isRunning(); break;
        case 1: *reinterpret_cast< bool*>(_v) = _t->isStereo(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->isPaused(); break;
        case 3: *reinterpret_cast< int*>(_v) = _t->frequencies(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject CApplication::staticMetaObject = {
    { &QApplication::staticMetaObject, qt_meta_stringdata_CApplication.data,
      qt_meta_data_CApplication,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CApplication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CApplication.stringdata0))
        return static_cast<void*>(const_cast< CApplication*>(this));
    return QApplication::qt_metacast(_clname);
}

int CApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 19;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 4;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 4;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void CApplication::isRunningChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void CApplication::isStereoChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void CApplication::isPausedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void CApplication::frequenciesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void CApplication::displayErrorSignal(QString _t1, QString _t2, bool _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void CApplication::displayInfoSignal(QString _t1, QString _t2, CAppDisplayInfoEnum _t3)
{
    void *_a[] = { Q_NULLPTR, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void CApplication::stopSignal()
{
    QMetaObject::activate(this, &staticMetaObject, 6, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE

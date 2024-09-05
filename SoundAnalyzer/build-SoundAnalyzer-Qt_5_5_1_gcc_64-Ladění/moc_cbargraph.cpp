/****************************************************************************
** Meta object code from reading C++ file 'cbargraph.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.5.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../dppp/Source/SoundAnalyzer/cbargraph.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'cbargraph.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.5.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_CBarGraph_t {
    QByteArrayData data[18];
    char stringdata0[222];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CBarGraph_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CBarGraph_t qt_meta_stringdata_CBarGraph = {
    {
QT_MOC_LITERAL(0, 0, 9), // "CBarGraph"
QT_MOC_LITERAL(1, 10, 12), // "colorChanged"
QT_MOC_LITERAL(2, 23, 0), // ""
QT_MOC_LITERAL(3, 24, 21), // "alternateColorChanged"
QT_MOC_LITERAL(4, 46, 14), // "visibleChanged"
QT_MOC_LITERAL(5, 61, 14), // "minimumChanged"
QT_MOC_LITERAL(6, 76, 14), // "maximumChanged"
QT_MOC_LITERAL(7, 91, 4), // "sync"
QT_MOC_LITERAL(8, 96, 7), // "cleanup"
QT_MOC_LITERAL(9, 104, 19), // "handleWindowChanged"
QT_MOC_LITERAL(10, 124, 13), // "QQuickWindow*"
QT_MOC_LITERAL(11, 138, 3), // "win"
QT_MOC_LITERAL(12, 142, 34), // "getApplicationInitializeFailS..."
QT_MOC_LITERAL(13, 177, 5), // "color"
QT_MOC_LITERAL(14, 183, 14), // "alternateColor"
QT_MOC_LITERAL(15, 198, 7), // "visible"
QT_MOC_LITERAL(16, 206, 7), // "minimum"
QT_MOC_LITERAL(17, 214, 7) // "maximum"

    },
    "CBarGraph\0colorChanged\0\0alternateColorChanged\0"
    "visibleChanged\0minimumChanged\0"
    "maximumChanged\0sync\0cleanup\0"
    "handleWindowChanged\0QQuickWindow*\0win\0"
    "getApplicationInitializeFailString\0"
    "color\0alternateColor\0visible\0minimum\0"
    "maximum"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CBarGraph[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       5,   70, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       5,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    0,   59,    2, 0x06 /* Public */,
       3,    0,   60,    2, 0x06 /* Public */,
       4,    0,   61,    2, 0x06 /* Public */,
       5,    0,   62,    2, 0x06 /* Public */,
       6,    0,   63,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       7,    0,   64,    2, 0x0a /* Public */,
       8,    0,   65,    2, 0x0a /* Public */,
       9,    1,   66,    2, 0x08 /* Private */,

 // methods: name, argc, parameters, tag, flags
      12,    0,   69,    2, 0x02 /* Public */,

 // signals: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 10,   11,

 // methods: parameters
    QMetaType::QVariant,

 // properties: name, type, flags
      13, QMetaType::QColor, 0x00495103,
      14, QMetaType::QColor, 0x00495103,
      15, QMetaType::Bool, 0x00495103,
      16, QMetaType::QReal, 0x00495103,
      17, QMetaType::QReal, 0x00495103,

 // properties: notify_signal_id
       0,
       1,
       2,
       3,
       4,

       0        // eod
};

void CBarGraph::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        CBarGraph *_t = static_cast<CBarGraph *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->colorChanged(); break;
        case 1: _t->alternateColorChanged(); break;
        case 2: _t->visibleChanged(); break;
        case 3: _t->minimumChanged(); break;
        case 4: _t->maximumChanged(); break;
        case 5: _t->sync(); break;
        case 6: _t->cleanup(); break;
        case 7: _t->handleWindowChanged((*reinterpret_cast< QQuickWindow*(*)>(_a[1]))); break;
        case 8: { QVariant _r = _t->getApplicationInitializeFailString();
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<int*>(_a[0]) = -1; break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<int*>(_a[0]) = -1; break;
            case 0:
                *reinterpret_cast<int*>(_a[0]) = qRegisterMetaType< QQuickWindow* >(); break;
            }
            break;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        void **func = reinterpret_cast<void **>(_a[1]);
        {
            typedef void (CBarGraph::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CBarGraph::colorChanged)) {
                *result = 0;
            }
        }
        {
            typedef void (CBarGraph::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CBarGraph::alternateColorChanged)) {
                *result = 1;
            }
        }
        {
            typedef void (CBarGraph::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CBarGraph::visibleChanged)) {
                *result = 2;
            }
        }
        {
            typedef void (CBarGraph::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CBarGraph::minimumChanged)) {
                *result = 3;
            }
        }
        {
            typedef void (CBarGraph::*_t)();
            if (*reinterpret_cast<_t *>(func) == static_cast<_t>(&CBarGraph::maximumChanged)) {
                *result = 4;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        CBarGraph *_t = static_cast<CBarGraph *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QColor*>(_v) = _t->color(); break;
        case 1: *reinterpret_cast< QColor*>(_v) = _t->alternateColor(); break;
        case 2: *reinterpret_cast< bool*>(_v) = _t->isVisible(); break;
        case 3: *reinterpret_cast< qreal*>(_v) = _t->minimum(); break;
        case 4: *reinterpret_cast< qreal*>(_v) = _t->maximum(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        CBarGraph *_t = static_cast<CBarGraph *>(_o);
        Q_UNUSED(_t)
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setColor(*reinterpret_cast< QColor*>(_v)); break;
        case 1: _t->setAlternateColor(*reinterpret_cast< QColor*>(_v)); break;
        case 2: _t->setVisible(*reinterpret_cast< bool*>(_v)); break;
        case 3: _t->setMinimum(*reinterpret_cast< qreal*>(_v)); break;
        case 4: _t->setMaximum(*reinterpret_cast< qreal*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

const QMetaObject CBarGraph::staticMetaObject = {
    { &QQuickItem::staticMetaObject, qt_meta_stringdata_CBarGraph.data,
      qt_meta_data_CBarGraph,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *CBarGraph::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CBarGraph::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_CBarGraph.stringdata0))
        return static_cast<void*>(const_cast< CBarGraph*>(this));
    if (!strcmp(_clname, "QOpenGLFunctions"))
        return static_cast< QOpenGLFunctions*>(const_cast< CBarGraph*>(this));
    return QQuickItem::qt_metacast(_clname);
}

int CBarGraph::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QQuickItem::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
#ifndef QT_NO_PROPERTIES
   else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 5;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 5;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void CBarGraph::colorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, Q_NULLPTR);
}

// SIGNAL 1
void CBarGraph::alternateColorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, Q_NULLPTR);
}

// SIGNAL 2
void CBarGraph::visibleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, Q_NULLPTR);
}

// SIGNAL 3
void CBarGraph::minimumChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, Q_NULLPTR);
}

// SIGNAL 4
void CBarGraph::maximumChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, Q_NULLPTR);
}
QT_END_MOC_NAMESPACE

/****************************************************************************
** Meta object code from reading C++ file 'CQSlider.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../include/CQSlider.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQSlider.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQSliderBase_t {
    QByteArrayData data[12];
    char stringdata0[127];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQSliderBase_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQSliderBase_t qt_meta_stringdata_CQSliderBase = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CQSliderBase"
QT_MOC_LITERAL(1, 13, 5), // "label"
QT_MOC_LITERAL(2, 19, 9), // "labelSize"
QT_MOC_LITERAL(3, 29, 10), // "valueLabel"
QT_MOC_LITERAL(4, 40, 13), // "valuePosition"
QT_MOC_LITERAL(5, 54, 13), // "ValuePosition"
QT_MOC_LITERAL(6, 68, 13), // "tickLabelSize"
QT_MOC_LITERAL(7, 82, 13), // "largeTickSize"
QT_MOC_LITERAL(8, 96, 13), // "smallTickSize"
QT_MOC_LITERAL(9, 110, 4), // "NONE"
QT_MOC_LITERAL(10, 115, 5), // "ABOVE"
QT_MOC_LITERAL(11, 121, 5) // "BELOW"

    },
    "CQSliderBase\0label\0labelSize\0valueLabel\0"
    "valuePosition\0ValuePosition\0tickLabelSize\0"
    "largeTickSize\0smallTickSize\0NONE\0ABOVE\0"
    "BELOW"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQSliderBase[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       7,   14, // properties
       1,   35, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::QString, 0x00095103,
       2, QMetaType::Double, 0x00095103,
       3, QMetaType::QString, 0x00095103,
       4, 0x80000000 | 5, 0x0009510b,
       6, QMetaType::Double, 0x00095103,
       7, QMetaType::Int, 0x00095103,
       8, QMetaType::Int, 0x00095103,

 // enums: name, alias, flags, count, data
       5,    5, 0x2,    3,   40,

 // enum data: key, value
       9, uint(CQSliderBase::ValuePosition::NONE),
      10, uint(CQSliderBase::ValuePosition::ABOVE),
      11, uint(CQSliderBase::ValuePosition::BELOW),

       0        // eod
};

void CQSliderBase::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CQSliderBase *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = _t->label(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->labelSize(); break;
        case 2: *reinterpret_cast< QString*>(_v) = _t->valueLabel(); break;
        case 3: *reinterpret_cast< ValuePosition*>(_v) = _t->valuePosition(); break;
        case 4: *reinterpret_cast< double*>(_v) = _t->tickLabelSize(); break;
        case 5: *reinterpret_cast< int*>(_v) = _t->largeTickSize(); break;
        case 6: *reinterpret_cast< int*>(_v) = _t->smallTickSize(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<CQSliderBase *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setLabel(*reinterpret_cast< QString*>(_v)); break;
        case 1: _t->setLabelSize(*reinterpret_cast< double*>(_v)); break;
        case 2: _t->setValueLabel(*reinterpret_cast< QString*>(_v)); break;
        case 3: _t->setValuePosition(*reinterpret_cast< ValuePosition*>(_v)); break;
        case 4: _t->setTickLabelSize(*reinterpret_cast< double*>(_v)); break;
        case 5: _t->setLargeTickSize(*reinterpret_cast< int*>(_v)); break;
        case 6: _t->setSmallTickSize(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQSliderBase::staticMetaObject = { {
    QMetaObject::SuperData::link<QSlider::staticMetaObject>(),
    qt_meta_stringdata_CQSliderBase.data,
    qt_meta_data_CQSliderBase,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQSliderBase::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQSliderBase::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQSliderBase.stringdata0))
        return static_cast<void*>(this);
    return QSlider::qt_metacast(_clname);
}

int CQSliderBase::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QSlider::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 7;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 7;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_CQSlider_t {
    QByteArrayData data[2];
    char stringdata0[24];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQSlider_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQSlider_t qt_meta_stringdata_CQSlider = {
    {
QT_MOC_LITERAL(0, 0, 8), // "CQSlider"
QT_MOC_LITERAL(1, 9, 14) // "tickLabelDelta"

    },
    "CQSlider\0tickLabelDelta"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQSlider[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       1,   14, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, QMetaType::Int, 0x00095103,

       0        // eod
};

void CQSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CQSlider *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< int*>(_v) = _t->tickLabelDelta(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<CQSlider *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setTickLabelDelta(*reinterpret_cast< int*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
    (void)_o;
    (void)_id;
    (void)_c;
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject CQSlider::staticMetaObject = { {
    QMetaObject::SuperData::link<CQSliderBase::staticMetaObject>(),
    qt_meta_stringdata_CQSlider.data,
    qt_meta_data_CQSlider,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQSlider.stringdata0))
        return static_cast<void*>(this);
    return CQSliderBase::qt_metacast(_clname);
}

int CQSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQSliderBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 1;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 1;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
struct qt_meta_stringdata_CQRealSlider_t {
    QByteArrayData data[16];
    char stringdata0[165];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQRealSlider_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQRealSlider_t qt_meta_stringdata_CQRealSlider = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CQRealSlider"
QT_MOC_LITERAL(1, 13, 12), // "valueChanged"
QT_MOC_LITERAL(2, 26, 0), // ""
QT_MOC_LITERAL(3, 27, 12), // "rangeChanged"
QT_MOC_LITERAL(4, 40, 16), // "valueChangedSlot"
QT_MOC_LITERAL(5, 57, 13), // "integerToReal"
QT_MOC_LITERAL(6, 71, 6), // "ivalue"
QT_MOC_LITERAL(7, 78, 4), // "snap"
QT_MOC_LITERAL(8, 83, 13), // "realToInteger"
QT_MOC_LITERAL(9, 97, 6), // "rvalue"
QT_MOC_LITERAL(10, 104, 7), // "minimum"
QT_MOC_LITERAL(11, 112, 7), // "maximum"
QT_MOC_LITERAL(12, 120, 10), // "singleStep"
QT_MOC_LITERAL(13, 131, 8), // "pageStep"
QT_MOC_LITERAL(14, 140, 9), // "precision"
QT_MOC_LITERAL(15, 150, 14) // "tickLabelDelta"

    },
    "CQRealSlider\0valueChanged\0\0rangeChanged\0"
    "valueChangedSlot\0integerToReal\0ivalue\0"
    "snap\0realToInteger\0rvalue\0minimum\0"
    "maximum\0singleStep\0pageStep\0precision\0"
    "tickLabelDelta"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQRealSlider[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       6,   58, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: name, argc, parameters, tag, flags
       1,    1,   39,    2, 0x06 /* Public */,
       3,    2,   42,    2, 0x06 /* Public */,

 // slots: name, argc, parameters, tag, flags
       4,    1,   47,    2, 0x08 /* Private */,
       5,    2,   50,    2, 0x08 /* Private */,
       8,    1,   55,    2, 0x08 /* Private */,

 // signals: parameters
    QMetaType::Void, QMetaType::Double,    2,
    QMetaType::Void, QMetaType::Double, QMetaType::Double,    2,    2,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Double, QMetaType::Int, QMetaType::Bool,    6,    7,
    QMetaType::Int, QMetaType::Double,    9,

 // properties: name, type, flags
      10, QMetaType::Double, 0x00095103,
      11, QMetaType::Double, 0x00095103,
      12, QMetaType::Double, 0x00095103,
      13, QMetaType::Double, 0x00095103,
      14, QMetaType::Int, 0x00095103,
      15, QMetaType::Double, 0x00095103,

       0        // eod
};

void CQRealSlider::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<CQRealSlider *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->valueChanged((*reinterpret_cast< double(*)>(_a[1]))); break;
        case 1: _t->rangeChanged((*reinterpret_cast< double(*)>(_a[1])),(*reinterpret_cast< double(*)>(_a[2]))); break;
        case 2: _t->valueChangedSlot((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: { double _r = _t->integerToReal((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< double*>(_a[0]) = std::move(_r); }  break;
        case 4: { int _r = _t->realToInteger((*reinterpret_cast< double(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    } else if (_c == QMetaObject::IndexOfMethod) {
        int *result = reinterpret_cast<int *>(_a[0]);
        {
            using _t = void (CQRealSlider::*)(double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQRealSlider::valueChanged)) {
                *result = 0;
                return;
            }
        }
        {
            using _t = void (CQRealSlider::*)(double , double );
            if (*reinterpret_cast<_t *>(_a[1]) == static_cast<_t>(&CQRealSlider::rangeChanged)) {
                *result = 1;
                return;
            }
        }
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CQRealSlider *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< double*>(_v) = _t->minimum(); break;
        case 1: *reinterpret_cast< double*>(_v) = _t->maximum(); break;
        case 2: *reinterpret_cast< double*>(_v) = _t->singleStep(); break;
        case 3: *reinterpret_cast< double*>(_v) = _t->pageStep(); break;
        case 4: *reinterpret_cast< int*>(_v) = _t->precision(); break;
        case 5: *reinterpret_cast< double*>(_v) = _t->tickLabelDelta(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<CQRealSlider *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setMinimum(*reinterpret_cast< double*>(_v)); break;
        case 1: _t->setMaximum(*reinterpret_cast< double*>(_v)); break;
        case 2: _t->setSingleStep(*reinterpret_cast< double*>(_v)); break;
        case 3: _t->setPageStep(*reinterpret_cast< double*>(_v)); break;
        case 4: _t->setPrecision(*reinterpret_cast< int*>(_v)); break;
        case 5: _t->setTickLabelDelta(*reinterpret_cast< double*>(_v)); break;
        default: break;
        }
    } else if (_c == QMetaObject::ResetProperty) {
    }
#endif // QT_NO_PROPERTIES
}

QT_INIT_METAOBJECT const QMetaObject CQRealSlider::staticMetaObject = { {
    QMetaObject::SuperData::link<CQSliderBase::staticMetaObject>(),
    qt_meta_stringdata_CQRealSlider.data,
    qt_meta_data_CQRealSlider,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQRealSlider::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQRealSlider::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQRealSlider.stringdata0))
        return static_cast<void*>(this);
    return CQSliderBase::qt_metacast(_clname);
}

int CQRealSlider::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = CQSliderBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 5)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 5;
    }
#ifndef QT_NO_PROPERTIES
    else if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 6;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 6;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}

// SIGNAL 0
void CQRealSlider::valueChanged(double _t1)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void CQRealSlider::rangeChanged(double _t1, double _t2)
{
    void *_a[] = { nullptr, const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t1))), const_cast<void*>(reinterpret_cast<const void*>(std::addressof(_t2))) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

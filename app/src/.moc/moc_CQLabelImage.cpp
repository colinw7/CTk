/****************************************************************************
** Meta object code from reading C++ file 'CQLabelImage.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../CQLabelImage.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CQLabelImage.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_CQLabelImage_t {
    QByteArrayData data[11];
    char stringdata0[69];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_CQLabelImage_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_CQLabelImage_t qt_meta_stringdata_CQLabelImage = {
    {
QT_MOC_LITERAL(0, 0, 12), // "CQLabelImage"
QT_MOC_LITERAL(1, 13, 4), // "type"
QT_MOC_LITERAL(2, 18, 4), // "Type"
QT_MOC_LITERAL(3, 23, 5), // "label"
QT_MOC_LITERAL(4, 29, 5), // "image"
QT_MOC_LITERAL(5, 35, 4), // "NONE"
QT_MOC_LITERAL(6, 40, 6), // "BOTTOM"
QT_MOC_LITERAL(7, 47, 3), // "TOP"
QT_MOC_LITERAL(8, 51, 4), // "LEFT"
QT_MOC_LITERAL(9, 56, 5), // "RIGHT"
QT_MOC_LITERAL(10, 62, 6) // "CENTER"

    },
    "CQLabelImage\0type\0Type\0label\0image\0"
    "NONE\0BOTTOM\0TOP\0LEFT\0RIGHT\0CENTER"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_CQLabelImage[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       3,   14, // properties
       1,   23, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // properties: name, type, flags
       1, 0x80000000 | 2, 0x0009510b,
       3, QMetaType::QString, 0x00095103,
       4, QMetaType::QImage, 0x00095103,

 // enums: name, alias, flags, count, data
       2,    2, 0x2,    6,   28,

 // enum data: key, value
       5, uint(CQLabelImage::Type::NONE),
       6, uint(CQLabelImage::Type::BOTTOM),
       7, uint(CQLabelImage::Type::TOP),
       8, uint(CQLabelImage::Type::LEFT),
       9, uint(CQLabelImage::Type::RIGHT),
      10, uint(CQLabelImage::Type::CENTER),

       0        // eod
};

void CQLabelImage::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{

#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty) {
        auto *_t = static_cast<CQLabelImage *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< Type*>(_v) = _t->type(); break;
        case 1: *reinterpret_cast< QString*>(_v) = _t->label(); break;
        case 2: *reinterpret_cast< QImage*>(_v) = _t->image(); break;
        default: break;
        }
    } else if (_c == QMetaObject::WriteProperty) {
        auto *_t = static_cast<CQLabelImage *>(_o);
        (void)_t;
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setType(*reinterpret_cast< Type*>(_v)); break;
        case 1: _t->setLabel(*reinterpret_cast< QString*>(_v)); break;
        case 2: _t->setImage(*reinterpret_cast< QImage*>(_v)); break;
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

QT_INIT_METAOBJECT const QMetaObject CQLabelImage::staticMetaObject = { {
    QMetaObject::SuperData::link<QFrame::staticMetaObject>(),
    qt_meta_stringdata_CQLabelImage.data,
    qt_meta_data_CQLabelImage,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *CQLabelImage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *CQLabelImage::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_CQLabelImage.stringdata0))
        return static_cast<void*>(this);
    return QFrame::qt_metacast(_clname);
}

int CQLabelImage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    
#ifndef QT_NO_PROPERTIES
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE

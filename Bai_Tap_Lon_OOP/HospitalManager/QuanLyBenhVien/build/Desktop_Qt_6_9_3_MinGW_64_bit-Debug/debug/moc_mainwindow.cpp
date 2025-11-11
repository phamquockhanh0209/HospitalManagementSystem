/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.9.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.9.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN10MainWindowE_t {};
} // unnamed namespace

template <> constexpr inline auto MainWindow::qt_create_metaobjectdata<qt_meta_tag_ZN10MainWindowE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "MainWindow",
        "onThemBenhNhan",
        "",
        "onSuaBenhNhan",
        "onXoaBenhNhan",
        "onXuatBenhNhan",
        "onThemBacSi",
        "onSuaBacSi",
        "onXoaBacSi",
        "onXuatBacSi",
        "onThemPhong",
        "onSuaPhong",
        "onXuatPhong",
        "onPhanCong",
        "onRaVien",
        "onThongKe"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onThemBenhNhan'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSuaBenhNhan'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onXoaBenhNhan'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onXuatBenhNhan'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onThemBacSi'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSuaBacSi'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onXoaBacSi'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onXuatBacSi'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onThemPhong'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSuaPhong'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onXuatPhong'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPhanCong'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRaVien'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onThongKe'
        QtMocHelpers::SlotData<void()>(15, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<MainWindow, qt_meta_tag_ZN10MainWindowE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10MainWindowE_t>.metaTypes,
    nullptr
} };

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->onThemBenhNhan(); break;
        case 1: _t->onSuaBenhNhan(); break;
        case 2: _t->onXoaBenhNhan(); break;
        case 3: _t->onXuatBenhNhan(); break;
        case 4: _t->onThemBacSi(); break;
        case 5: _t->onSuaBacSi(); break;
        case 6: _t->onXoaBacSi(); break;
        case 7: _t->onXuatBacSi(); break;
        case 8: _t->onThemPhong(); break;
        case 9: _t->onSuaPhong(); break;
        case 10: _t->onXuatPhong(); break;
        case 11: _t->onPhanCong(); break;
        case 12: _t->onRaVien(); break;
        case 13: _t->onThongKe(); break;
        default: ;
        }
    }
    (void)_a;
}

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10MainWindowE_t>.strings))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 14)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 14;
    }
    return _id;
}
QT_WARNING_POP

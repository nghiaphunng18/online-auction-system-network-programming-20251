/****************************************************************************
** Meta object code from reading C++ file 'mainwindow.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../mainwindow.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mainwindow.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.1. It"
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
        "onConnectClicked",
        "",
        "onDisconnectClicked",
        "onBackToConnect",
        "onLoginClicked",
        "onRefreshRooms",
        "onCreateRoom",
        "onJoinRoom",
        "onJoinSelected",
        "onOpenHistoryTab",
        "onRefreshHistory",
        "onRoomFilterChanged",
        "onRoomsSelectionChanged",
        "onSelRoomUserSearchChanged",
        "onSelRoomUserDoubleClicked",
        "QListWidgetItem*",
        "item",
        "onHistoryFilterChanged",
        "onLogout",
        "onLeaveRoom",
        "onStartRoom",
        "onEndRoom",
        "onViewQueued",
        "onViewEnded",
        "onAddDraft",
        "onDraftList",
        "onPublishOk",
        "onBid",
        "onBuyNow",
        "onRefreshChatUsers",
        "onChatSend",
        "onRoomChatSend",
        "onSockConnected",
        "onSockDisconnected",
        "onSockError",
        "msg",
        "onLine",
        "line"
    };

    QtMocHelpers::UintData qt_methods {
        // Slot 'onConnectClicked'
        QtMocHelpers::SlotData<void()>(1, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDisconnectClicked'
        QtMocHelpers::SlotData<void()>(3, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBackToConnect'
        QtMocHelpers::SlotData<void()>(4, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLoginClicked'
        QtMocHelpers::SlotData<void()>(5, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRefreshRooms'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onCreateRoom'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onJoinRoom'
        QtMocHelpers::SlotData<void()>(8, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onJoinSelected'
        QtMocHelpers::SlotData<void()>(9, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onOpenHistoryTab'
        QtMocHelpers::SlotData<void()>(10, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRefreshHistory'
        QtMocHelpers::SlotData<void()>(11, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRoomFilterChanged'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRoomsSelectionChanged'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSelRoomUserSearchChanged'
        QtMocHelpers::SlotData<void()>(14, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSelRoomUserDoubleClicked'
        QtMocHelpers::SlotData<void(QListWidgetItem *)>(15, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 16, 17 },
        }}),
        // Slot 'onHistoryFilterChanged'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLogout'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onLeaveRoom'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onStartRoom'
        QtMocHelpers::SlotData<void()>(21, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onEndRoom'
        QtMocHelpers::SlotData<void()>(22, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onViewQueued'
        QtMocHelpers::SlotData<void()>(23, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onViewEnded'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onAddDraft'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onDraftList'
        QtMocHelpers::SlotData<void()>(26, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onPublishOk'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBid'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onBuyNow'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRefreshChatUsers'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onChatSend'
        QtMocHelpers::SlotData<void()>(31, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRoomChatSend'
        QtMocHelpers::SlotData<void()>(32, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSockConnected'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSockDisconnected'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSockError'
        QtMocHelpers::SlotData<void(const QString &)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 36 },
        }}),
        // Slot 'onLine'
        QtMocHelpers::SlotData<void(const QString &)>(37, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 38 },
        }}),
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
        case 0: _t->onConnectClicked(); break;
        case 1: _t->onDisconnectClicked(); break;
        case 2: _t->onBackToConnect(); break;
        case 3: _t->onLoginClicked(); break;
        case 4: _t->onRefreshRooms(); break;
        case 5: _t->onCreateRoom(); break;
        case 6: _t->onJoinRoom(); break;
        case 7: _t->onJoinSelected(); break;
        case 8: _t->onOpenHistoryTab(); break;
        case 9: _t->onRefreshHistory(); break;
        case 10: _t->onRoomFilterChanged(); break;
        case 11: _t->onRoomsSelectionChanged(); break;
        case 12: _t->onSelRoomUserSearchChanged(); break;
        case 13: _t->onSelRoomUserDoubleClicked((*reinterpret_cast<std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 14: _t->onHistoryFilterChanged(); break;
        case 15: _t->onLogout(); break;
        case 16: _t->onLeaveRoom(); break;
        case 17: _t->onStartRoom(); break;
        case 18: _t->onEndRoom(); break;
        case 19: _t->onViewQueued(); break;
        case 20: _t->onViewEnded(); break;
        case 21: _t->onAddDraft(); break;
        case 22: _t->onDraftList(); break;
        case 23: _t->onPublishOk(); break;
        case 24: _t->onBid(); break;
        case 25: _t->onBuyNow(); break;
        case 26: _t->onRefreshChatUsers(); break;
        case 27: _t->onChatSend(); break;
        case 28: _t->onRoomChatSend(); break;
        case 29: _t->onSockConnected(); break;
        case 30: _t->onSockDisconnected(); break;
        case 31: _t->onSockError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 32: _t->onLine((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
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
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 33)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 33;
    }
    return _id;
}
QT_WARNING_POP

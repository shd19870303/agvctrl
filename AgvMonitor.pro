QT       += core gui
QT       += sql
QT       += network
QT       += network axcontainer
#QT       += core5compat
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

include(./qtpropertybrowser/src/qtpropertybrowser.pri)

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    addtaskview.cpp \
    agvctrl_tool.cpp \
    arrow_action.cpp \
    batteryinfo_widget.cpp \
    bmachinecontrol.cpp \
    car.cpp \
    car_info.cpp \
    carsizewidget.cpp \
    cartrans_action.cpp \
    commands.cpp \
    cxjbackdialog.cpp \
    cxjdatadialog.cpp \
    drawobj.cpp \
    drawscene.cpp \
    drawtool.cpp \
    drawview.cpp \
    driftinput.cpp \
    driftinputradius.cpp \
    find.cpp \
    float_rectitem.cpp \
    floating_dialog.cpp \
    floatingwindow.cpp \
    inputcreatetasksum.cpp \
    main.cpp \
    mainwindow.cpp \
    map.cpp \
    monitor.cpp \
    moveitem.cpp \
    mydebugwidget.cpp \
    mylineeditctrl.cpp \
    playback_tool.cpp \
    pointsize_action.cpp \
    protoFile/agv_map.pb.cc \
    protoFile/agv_mon.pb.cc \
    protoFile/batteryMonitor.pb.cc \
    registrydialog.cpp \
    rotateitem.cpp \
    rulebar.cpp \
    sa_backgrounditem.cpp \
    segment_action.cpp \
    sizehandle.cpp \
    station_backgrounditem.cpp \
    subwidget.cpp \
    tablenumberitem.cpp \
    taskdialog.cpp \
    taskspecialdialog.cpp \
    updatestation.cpp \
    wait_dialog.cpp

HEADERS += \
    addtaskview.h \
    agvctrl_tool.h \
    arrow_action.h \
    batteryinfo_widget.h \
    bmachinecontrol.h \
    car.h \
    car_info.h \
    carsizewidget.h \
    cartrans_action.h \
    checksum.h \
    commands.h \
    cxjbackdialog.h \
    cxjdatadialog.h \
    drawobj.h \
    drawscene.h \
    drawtool.h \
    drawview.h \
    driftinput.h \
    driftinputradius.h \
    find.h \
    float_rectitem.h \
    floating_dialog.h \
    floatingwindow.h \
    inputcreatetasksum.h \
    mainwindow.h \
    map.h \
    monitor.h \
    moveitem.h \
    mydebugwidget.h \
    mylineeditctrl.h \
    playback_tool.h \
    pointsize_action.h \
    protoFile/agv_map.pb.h \
    protoFile/agv_mon.pb.h \
    protoFile/batteryMonitor.pb.h \
    registrydialog.h \
    rotateitem.h \
    rulebar.h \
    sa_backgrounditem.h \
    segment_action.h \
    sizehandle.h \
    station_backgrounditem.h \
    subwidget.h \
    tablenumberitem.h \
    taskdialog.h \
    taskspecialdialog.h \
    updatestation.h \
    wait_dialog.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    monitor.qrc \
    scale.qrc

DISTFILES += \
    agv_map.proto \
    agv_mon.proto \
    batteryMonitor.proto

FORMS += \
    addtaskview.ui \
    agvctrl_tool.ui \
    batteryinfo_widget.ui \
    carsizewidget.ui \
    cxjbackdialog.ui \
    cxjdatadialog.ui \
    driftinput.ui \
    driftinputradius.ui \
    find.ui \
    floating_dialog.ui \
    floatingwindow.ui \
    inputcreatetasksum.ui \
    monitor.ui \
    moveitem.ui \
    mydebugwidget.ui \
    playback_tool.ui \
    registrydialog.ui \
    rotateitem.ui \
    subwidget.ui \
    taskdialog.ui \
    taskspecialdialog.ui \
    updatestation.ui \
    wait_dialog.ui

RC_ICONS = monitor.ico

TARGET = AgvMonitor




#win32: LIBS += -LC:/Users/34574/Desktop/Work/proto_build/lib/ -lprotobuf

#INCLUDEPATH += C:/Users/34574/Desktop/Work/proto_build
#DEPENDPATH += C:/Users/34574/Desktop/Work/proto_build

#win32:!win32-g++: PRE_TARGETDEPS += C:/Users/34574/Desktop/Work/proto_build/lib/protobuf.lib
#else:win32-g++: PRE_TARGETDEPS += C:/Users/34574/Desktop/Work/proto_build/lib/libprotobuf.a

RESOURCES += qdarkstyle/

win32: LIBS += -L$$PWD/protobufLib/lib/ -lprotobuf

INCLUDEPATH += $$PWD/protobufLib/inc
DEPENDPATH += $$PWD/protobufLib

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/protobufLib/lib/protobuf.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/protobufLib/lib/libprotobuf.a

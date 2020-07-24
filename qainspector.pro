# Copyright (c) 2019-2020 Open Mobile Platform LLC.
QT += quick network xml xmlpatterns widgets
CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    PaintedWidget.cpp \
    main.cpp \
    mytreemodel.cpp \
    socketconnector.cpp \
    iteminfodialog.cpp \
    treeviewdialog.cpp \
    mytreemodel2.cpp

HEADERS += \
    PaintedWidget.hpp \
    mytreemodel.h \
    socketconnector.h \
    iteminfodialog.h \
    treeviewdialog.h \
    mytreemodel2.h

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


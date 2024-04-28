QT       += core gui

QT += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dialog_importdata.cpp \
    dialog_initdays.cpp \
    dialog_personalinfo.cpp \
    dialog_search.cpp \
    dialog_status.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    dialog_importdata.h \
    dialog_initdays.h \
    dialog_personalinfo.h \
    dialog_search.h \
    dialog_status.h \
    mainwindow.h

FORMS += \
    dialog_importdata.ui \
    dialog_initdays.ui \
    dialog_personalinfo.ui \
    dialog_search.ui \
    dialog_status.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

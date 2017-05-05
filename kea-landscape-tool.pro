#-------------------------------------------------
#
# Project created by QtCreator 2017-05-05T21:03:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = bin/kea-landscape-tool
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += src/main.cpp \
    src/mainwindow.cpp

HEADERS  += src/mainwindow.h

FORMS    += src/mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../lib/soplex-3.0.0/lib/release/ -lsoplex -lz -lgmp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../lib/soplex-3.0.0/lib/debug/ -lsoplex -lz -lgmp
else:unix: LIBS += -L$$PWD/../../lib/soplex-3.0.0/lib/ -lsoplex -lz -lgmp

INCLUDEPATH += $$PWD/../../lib/soplex-3.0.0/src
DEPENDPATH += $$PWD/../../lib/soplex-3.0.0/src

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/soplex-3.0.0/lib/release/libsoplex.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/soplex-3.0.0/lib/debug/libsoplex.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/soplex-3.0.0/lib/release/soplex.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../lib/soplex-3.0.0/lib/debug/soplex.lib
else:unix: PRE_TARGETDEPS += $$PWD/../../lib/soplex-3.0.0/lib/libsoplex.a

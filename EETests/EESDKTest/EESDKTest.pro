#-------------------------------------------------
#
# Project created by QtCreator 2018-02-07T17:44:43
#
#-------------------------------------------------

QT       += core network widgets

QT       -= gui

TARGET = tst_eesdktest
CONFIG   += console
CONFIG   -= app_bundle
CONFIG += c++11

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include (../../EESDK/EESDK.pri)
include (../../EEDataSync/EEDataSync.pri)

SOURCES += \
    main.cpp \
    eesdktest.cpp

DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += $$PWD/../..

INCLUDEPATH += $$PWD/../../EEDataSync/EEParser/

HEADERS += \
    eesdktest.h



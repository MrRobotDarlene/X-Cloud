#-------------------------------------------------
#
# Project created by QtCreator 2018-01-18T16:39:29
#
#-------------------------------------------------

QT       += core gui network webenginewidgets websockets webchannel

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

unix:!macx {
	TARGET = "xcloud"
}
macx {
	TARGET = "X Cloud"
}

win32 {
        TARGET = "X Cloud"
}
TEMPLATE = app

CONFIG += c++11 \
          openssl-linked \
          openssl


unix:!macx {
    target.files += ./xcloud
    target.path += /usr/bin/

    desktop_config.files += resources/xcloud.desktop
    desktop_config.path += /usr/share/applications/

    logo_config.files += $$PWD/resources/xcloud.png
    logo_config.path += /usr/share/pixmaps/


    INSTALLS += target \
                desktop_config \
                logo_config
}


win32: {
    RC_FILE = xcloud.rc
}

macx: {
    ICON = resources/xcloud.icns
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

include(EESDK/EESDK.pri)
include(EEDataSync/EEDataSync.pri)
include(EEWidget/EEWidget.pri)

SOURCES +=  main.cpp \
    eemaincontroller.cpp \
    eesettingsclass.cpp \
    eelog.cpp

HEADERS += \
    eemaincontroller.h \
    eesettingsclass.h \
    eelog.h \
    globalconstants.h

RESOURCES += \
    resources.qrc

INCLUDEPATH += $$PWD/EEDataSync/EELocalDataParser/
INCLUDEPATH += $$PWD/EEDataSync/EEDataManager/
INCLUDEPATH += $$PWD/EEDataSync/EELocalDataParser/
INCLUDEPATH += $$PWD/EEDataSync/
INCLUDEPATH += $$PWD/EEBucketSyncronization/
INCLUDEPATH += $$PWD/EESDK/EEContainers/
INCLUDEPATH += $$PWD/EESDK/EECrypto/
INCLUDEPATH += $$PWD/EESDK/EECrypto/EESignInCrypto/
INCLUDEPATH += $$PWD/EESDK/EECrypto/EEDataCrypto/


DISTFILES += \
    resources/xcloud.desktop

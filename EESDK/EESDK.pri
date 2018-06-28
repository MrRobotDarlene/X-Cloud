#-------------------------------------------------
#
# Project created by QtCreator 2018-01-18T16:39:29
#
#-------------------------------------------------

SOURCES += \
    $$PWD/eesdkparser.cpp \
    $$PWD/EEContainers/eebucket.cpp \
    $$PWD/EEContainers/eeabstractcontainer.cpp \
    $$PWD/EEContainers/eefile.cpp \
    $$PWD/EEContainers/eehmac.cpp \
    $$PWD/EEContainers/eeuser.cpp \
    $$PWD/EEContainers/eeerror.cpp \
    $$PWD/eesdk.cpp \
    $$PWD/eefileloader.cpp \
    $$PWD/eecallbackcontroller.cpp \
    $$PWD/EECrypto/EEDataCrypto/bip39.c \
    $$PWD/EECrypto/EEDataCrypto/controller.c \
    $$PWD/EECrypto/EEDataCrypto/crypto.c \
    $$PWD/EECrypto/EEDataCrypto/csdk.c \
    $$PWD/EECrypto/EEDataCrypto/downloader.c \
    $$PWD/EECrypto/EEDataCrypto/http.c \
    $$PWD/EECrypto/EEDataCrypto/rs.c \
    $$PWD/EECrypto/EEDataCrypto/uploader.c \
    $$PWD/EECrypto/EEDataCrypto/utils.c \
    $$PWD/eesignincontroller.cpp

HEADERS += \ 
    $$PWD/eesdkparser.h \
    $$PWD/EEContainers/eebucket.h \
    $$PWD/EEContainers/eeabstractcontainer.h \
    $$PWD/EEContainers/EEContainers.h \
    $$PWD/EEContainers/eefile.h \
    $$PWD/EEContainers/eehmac.h \
    $$PWD/EEContainers/eeuser.h \
    $$PWD/EEContainers/eeerror.h \
    $$PWD/eesdk.h \
    $$PWD/eefileloader.h \
    $$PWD/eecallbackcontroller.h \
    $$PWD/EECrypto/EEDataCrypto/bip39.h \
    $$PWD/EECrypto/EEDataCrypto/bip39_english.h \
    $$PWD/EECrypto/EEDataCrypto/controller.h \
    $$PWD/EECrypto/EEDataCrypto/crypto.h \
    $$PWD/EECrypto/EEDataCrypto/crypto_files.hpp \
    $$PWD/EECrypto/EEDataCrypto/csdk.h \
    $$PWD/EECrypto/EEDataCrypto/downloader.h \
    $$PWD/EECrypto/EEDataCrypto/http.h \
    $$PWD/EECrypto/EEDataCrypto/rs.h \
    $$PWD/EECrypto/EEDataCrypto/uploader.h \
    $$PWD/EECrypto/EEDataCrypto/utils.h \
    $$PWD/eesignincontroller.h
unix:!macx {
    LIBS += -L/usr/local/lib/ -lnettle -lcurl -ljson-c -luuid -luv
    INCLUDEPATH += /usr/local/include
}

macx: {
    LIBS += -L/usr/local/lib/ -lnettle -lcurl -ljson-c -luuid -luv
    INCLUDEPATH += /usr/local/include
}

win32: {
    LIBS += $$PWD/../3rdpartylibs/lib/ncrypt.dll.a
    LIBS += $$PWD/../3rdpartylibs/lib/libnettle.dll.a
    LIBS += $$PWD/../3rdpartylibs/lib/libjson-c.dll.a

    LIBS += $$PWD/../3rdpartylibs/lib/libgnutlsxx.dll.a
    LIBS += $$PWD/../3rdpartylibs/lib/libgnutls.dll.a
    LIBS += $$PWD/../3rdpartylibs/lib/libcurl.dll.a
    LIBS += $$PWD/../3rdpartylibs/lib/libgmp.dll.a
    LIBS += $$PWD/../3rdpartylibs/lib/libuv.dll.a

    INCLUDEPATH += $$PWD/../3rdpartylibs/include
    DEPENDPATH += $$PWD/../3rdpartylibs/include
}

INCLUDEPATH += $$PWD/EECrypto/EEDataCrypto/

INCLUDEPATH += $$PWD/../EEDataSync/EELocalDataParser/
INCLUDEPATH += $$PWD/EEDataSync/EELocalDataParser/



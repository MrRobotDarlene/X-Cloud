#-------------------------------------------------
#
# Project created by QtCreator 2018-01-18T16:39:29
#
#-------------------------------------------------

SOURCES += \
    $$PWD/eesdkparser.cpp \
    $$PWD/EEContainers/eebucket.cpp \
    $$PWD/EEContainers/eeframe.cpp \
    $$PWD/EEContainers/eeabstractcontainer.cpp \
    $$PWD/EEContainers/eetoken.cpp \
    $$PWD/EEContainers/eeshard.cpp \
    $$PWD/EEContainers/eefarmer.cpp \
    $$PWD/EEContainers/eefile.cpp \
    $$PWD/EEContainers/eehmac.cpp \
    $$PWD/EEContainers/eeuser.cpp \
    $$PWD/EEContainers/eeerror.cpp \
    $$PWD/eesdk.cpp \
    $$PWD/eefileloader.cpp \
    $$PWD/EEContainers/eeshardbasic.cpp \
    $$PWD/EEContainers/eeshardrequest.cpp \
    $$PWD/EECrypto/bip39.c \
    $$PWD/EECrypto/crypto.c \
    $$PWD/EECrypto/utils.c \
    $$PWD/EECrypto/http.c \
    $$PWD/EECrypto/uploader.c \
    $$PWD/EECrypto/downloader.c \
    $$PWD/EECrypto/rs.c \
    $$PWD/EECrypto/controller.c \
    $$PWD/eecallbackcontroller.cpp \
    $$PWD/EECrypto/csdk.c

HEADERS += \ 
    $$PWD/eesdkparser.h \
    $$PWD/EEContainers/eebucket.h \
    $$PWD/EEContainers/eeframe.h \
    $$PWD/EEContainers/eeabstractcontainer.h \
    $$PWD/EEContainers/eetoken.h \
    $$PWD/EEContainers/eeshard.h \
    $$PWD/EEContainers/eefarmer.h \
    $$PWD/EEContainers/eecontainers.h \
    $$PWD/EEContainers/eefile.h \
    $$PWD/EEContainers/eehmac.h \
    $$PWD/EEContainers/eeuser.h \
    $$PWD/EEContainers/eeerror.h \
    $$PWD/eesdk.h \
    $$PWD/eefileloader.h \
    $$PWD/EEContainers/eeshardbasic.h \
    $$PWD/EEContainers/eeshardrequest.h \
    $$PWD/EECrypto/bip39.h \
    $$PWD/EECrypto/bip39_english.h \
    $$PWD/EECrypto/crypto.h \
    $$PWD/EECrypto/utils.h \
    $$PWD/EECrypto/http.h \
    $$PWD/EECrypto/crypto_files.hpp \
    $$PWD/EECrypto/uploader.h \
    $$PWD/EECrypto/downloader.h \
    $$PWD/EECrypto/rs.h \
    $$PWD/EECrypto/controller.h \
    $$PWD/eecallbackcontroller.h \
    $$PWD/EECrypto/csdk.h


unix:!macx {
    LIBS += -L/usr/local/lib/ -lnettle -lcurl -ljson-c -luuid -luv
}

macx: {
    LIBS += -L/opt/local/lib/ -lnettle -lcurl -ljson-c -luuid -luv
}

INCLUDEPATH += $$PWD/EECrypto/
INCLUDEPATH += /usr/local/include

INCLUDEPATH += $$PWD ../EEDataSync/EEParser/


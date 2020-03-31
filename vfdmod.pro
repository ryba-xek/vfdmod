#-------------------------------------------------
#
# Project created by QtCreator 2020-03-02T20:44:55
#
#-------------------------------------------------

QT       += core
QT       -= gui

CONFIG   += console
CONFIG   -= app_bundle

TARGET = vfdmod
DEFINES += APP_TARGET=\\\"$$TARGET\\\"

TEMPLATE = app

VERSION = 0.2.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

INCLUDEPATH += /usr/include/linuxcnc
LIBS += -lmodbus -llinuxcnchal

SOURCES += main.cpp \
    load-config.cpp \
    make-config.cpp

HEADERS += \
    default-values.h \
    structures.h

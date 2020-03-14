#-------------------------------------------------
#
# Project created by QtCreator 2020-03-02T20:44:54
#
#-------------------------------------------------

QT       += core
QT       -= gui

CONFIG   += console
CONFIG   -= app_bundle

TARGET = vfdmod
DEFINES += APP_TARGET=\\\"$$TARGET\\\"

TEMPLATE = app

VERSION = 0.1.0
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

INCLUDEPATH += /usr/include/linuxcnc
LIBS += -lmodbus -llinuxcnchal

SOURCES += main.cpp \
    write-config.cpp \
    load-config.cpp

HEADERS += \
    default-values.h \
    structures.h

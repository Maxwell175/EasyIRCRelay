QT += core
QT += network
QT -= gui

CONFIG += c++11

TARGET = EasyIRCRelay
CONFIG += console
CONFIG -= app_bundle

include(/home/maxwell/libcommuni-3.5.0/src/src.pri)

TEMPLATE = app

SOURCES += main.cpp \
    mainworker.cpp \
    sigwatch.cpp

HEADERS += \
    mainworker.h \
    sigwatch.h

DISTFILES += config.json


copydata.commands = $(COPY) $$PWD/config.json $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata

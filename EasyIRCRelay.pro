# EasyIRCRelay
# Copyright 2016 (c) - Maxwell Dreytser
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

message("Building EasyIRCRelay...")
message("Copyright (c) 2016 - Maxwell Dreytser")
message("LibCommuni, a copy of which is included, is copyright of its owner.")

QT += core
QT += network
QT -= gui

CONFIG += c++11

TARGET = EasyIRCRelay
CONFIG += console
CONFIG -= app_bundle

include(libcommuni-3.5.0/src/src.pri)

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

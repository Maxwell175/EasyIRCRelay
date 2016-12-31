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
message("")
message("Configuration:")
message("BINDIR  - The directory where the compiled executable will be located.")
message("    Default: /usr/local/bin")
message("CONFDIR - The directory where the config folder should be created.")
message("    Config will be installed to <CONFDIR>/EasyIRCRelay.")
message("    Default: /etc")
message("NOINITD - Disables the installation as a service. (set to any non-empty value)")
message("RUNAS   - (Required!) Specifies the user the service will run as. (only if NOINITD isn't set)")
message("    Note: It is not recommended to run as root!")
message("")
message("Example: qmake BINDIR=/usr/bin NOINITD=true")
message("Example: qmake BINDIR=/opt CONFDIR=/opt/etc RUNAS=relayuser")

isEmpty(BINDIR) {
    CONFIG(debug, debug|release){
        BINDIR = $$OUT_PWD
    } else {
        BINDIR = /usr/local/bin
    }
}

isEmpty(CONFDIR) {
    CONFIG(debug, debug|release){
        CONFDIR = $$OUT_PWD
    } else {
        CONFDIR = /etc
    }
}

isEmpty(INITDDIR) {
    CONFIG(debug, debug|release){
        INITDDIR = $$OUT_PWD
    } else {
        INITDDIR = /etc/init.d
    }
}


QT += core
QT += network
QT -= gui

CONFIG += c++11

TARGET = EasyIRCRelay
TARGET.path = $$BINDIR
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

DISTFILES += config.json \
    EasyIRCRelay.init.m4

copydata.commands = install -C -m 644 $$PWD/config.json $$OUT_PWD/config.json | true
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata


binInstall.commands = install -C -D $$OUT_PWD/EasyIRCRelay $$BINDIR/EasyIRCRelay

appConfigInstall.commands = install -b -C -D -m 644 $$OUT_PWD/config.json $$CONFDIR/EasyIRCRelay/config.json

install.depends = binInstall
install.depends += appConfigInstall

QMAKE_EXTRA_TARGETS += install binInstall appConfigInstall

isEmpty(NOINITD) {
    CONFIG(debug, debug|release){
        RUNAS = $$USER
    } else {
        isEmpty(RUNAS) {
            RUNAS = $$prompt("What user should the program will run as")
        }
    }

    genInitD.target = $$PWD/EasyIRCRelay.init.m4
    getInitD.commands = "m4 -D \"_RUNASUSERNAME_=$$RUNAS\" -D \"_BINPREFIX_=$$BINDIR\" -D \"_ETCPREFIX_=$$CONFDIR\" $$PWD/EasyIRCRelay.init.m4 > $$OUT_PWD/EasyIRCRelay.init"

    first.depends += getInitD
    export(first.depends)
    export(getInitD.commands)
    QMAKE_EXTRA_TARGETS += first getInitD


    InitDInstall.commands = install -C -D EasyIRCRelay.init $$INITDDIR/EasyIRCRelay
    InitDLogInstall.commands = touch /var/log/EasyIRCRelay.log && chown \"$$RUNAS\" /var/log/EasyIRCRelay.log
    install.depends += InitDInstall InitDLogInstall
    QMAKE_EXTRA_TARGETS += install InitDInstall InitDLogInstall
}

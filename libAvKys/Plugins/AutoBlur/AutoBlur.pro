# Webcamoid, webcam capture application.
# Copyright (C) 2016  Gonzalo Exequiel Pedone
#
# Webcamoid is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Webcamoid is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Webcamoid. If not, see <http://www.gnu.org/licenses/>.
#
# Web-Site: http://webcamoid.github.io/

exists(translations.qrc) {
    TRANSLATIONS = $$files(share/ts/*.ts)
    RESOURCES += translations.qrc
}

exists(akcommons.pri) {
    include(akcommons.pri)
} else {
    exists(../../akcommons.pri) {
        include(../../akcommons.pri)
    } else {
        error("akcommons.pri file not found.")
    }
}

CONFIG += plugin
CONFIG += link_pkgconfig
PKGCONFIG += opencv4

HEADERS = \
    src/autoblur.h \
    src/blurelement.h

INCLUDEPATH += \
    ../../Lib/src

LIBS += -L$${OUT_PWD}/../../Lib/$${BIN_DIR} -l$${COMMONS_TARGET} -lopencv_core -lopencv_video

OTHER_FILES += pspec.json

QT += qml

RESOURCES += \
    AutoBlur.qrc

SOURCES = \
    src/autoblur.cpp \
    src/blurelement.cpp

lupdate_only {
    SOURCES += $$files(share/qml/*.qml)
}

DESTDIR = $${OUT_PWD}/$${BIN_DIR}
android: TARGET = $${COMMONS_TARGET}_lib$${TARGET}

TEMPLATE = lib

INSTALLS += target
target.path = $${INSTALLPLUGINSDIR}

TEMPLATE = app
CONFIG += console c++17
CONFIG -= qt

DESTDIR = $$PWD/../../build

QMAKE_CXXFLAGS_DEBUG *= -O
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

LIBS += -L$$PWD/../../build -lLouvre -lGLESv2
LIBS += -lSOIL
INCLUDEPATH += /usr/include/drm
INCLUDEPATH += ../../lib/
INCLUDEPATH += ../../lib/classes/
INCLUDEPATH += /usr/share/

QMAKE_LFLAGS += -Wl,-rpath=.

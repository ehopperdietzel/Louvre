TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += ordered
TARGET = MyCompositor

QMAKE_CXXFLAGS_RELEASE -= -O
#QMAKE_CXXFLAGS_RELEASE -= -O1
#QMAKE_CXXFLAGS_RELEASE -= -O2
#QMAKE_CXXFLAGS_RELEASE *= -O3

LIBS += -lGLESv2
LIBS += -L$$PWD/../../lib/build -lLouvre
INCLUDEPATH += /usr/include/drm
INCLUDEPATH += ../../lib/
INCLUDEPATH += ../../lib/classes/
INCLUDEPATH += ../../lib/globals/Wayland/
INCLUDEPATH += ../../lib/globals/XdgShell/
INCLUDEPATH += /usr/share/

QMAKE_LFLAGS += -Wl,-rpath=../../../src/lib/build

DISTFILES += \
    meson.build \
    res/images/Wallpaper.png \
    res/shaders/Fragment.glsl \
    res/shaders/Vertex.glsl

HEADERS += \
    Compositor.h

SOURCES += \
    Compositor.cpp \
    main.cpp

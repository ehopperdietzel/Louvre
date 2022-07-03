TEMPLATE = app
CONFIG += console c++17
CONFIG -= qt
TARGET = MyCompositor

DESTDIR = $$PWD/../../../build

QMAKE_CXXFLAGS_DEBUG *= -O
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

LIBS += -lGLESv2 -L/usr/local/lib/x86_64-linux-gnu -lwayland-server
#LIBS += -L$$PWD/../../lib/build -lLouvre
LIBS += -L$$PWD/../../../build -lLouvre
INCLUDEPATH += /usr/include/drm
INCLUDEPATH += ../../lib/
INCLUDEPATH += ../../lib/classes/
INCLUDEPATH += ../../lib/globals/Wayland/
INCLUDEPATH += ../../lib/globals/XdgShell/
INCLUDEPATH += /usr/share/

#QMAKE_LFLAGS += -Wl,-rpath=../../../src/lib/build
QMAKE_LFLAGS += -Wl,-rpath=.
DISTFILES += \
    meson.build \
    res/images/Wallpaper.png \
    res/shaders/Fragment.glsl \
    res/shaders/Vertex.glsl

HEADERS += \
    Compositor.h \
    Output.h \
    Surface.h

SOURCES += \
    Compositor.cpp \
    Output.cpp \
    Surface.cpp \
    main.cpp

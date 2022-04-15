TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += ordered
TARGET = MyCompositor

LIBS += -lGLESv2 -lSOIL
LIBS += -L../../src/build -lWpp
INCLUDEPATH += /usr/include/drm
INCLUDEPATH += ../../src/
INCLUDEPATH += ../../src/classes/
INCLUDEPATH += ../../src/globals/Wayland/
INCLUDEPATH += ../../src/globals/XdgShell/
INCLUDEPATH += /usr/share/

copydata.commands = $(COPY_DIR) $$PWD/res $$OUT_PWD
copydata2.commands = $(COPY) $$PWD/../../src/build/* $$OUT_PWD
first.depends = $(first) copydata copydata2
export(first.depends)
export(copydata.commands)
export(copydata2.commands)
QMAKE_EXTRA_TARGETS += first copydata copydata2


QMAKE_LFLAGS += -Wl,-rpath=.

SOURCES += \
        MyClient.cpp \
        MyCompositor.cpp \
        MyOutputManager.cpp \
        MySurface.cpp \
        main.cpp

HEADERS += \
    MyClient.h \
    MyCompositor.h \
    MyOutputManager.h \
    MySurface.h

DISTFILES += \
    shaders/Fragment.glsl \
    shaders/Vertex.glsl



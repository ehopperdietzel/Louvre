TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lwayland-server -lEGL -lGL -ldrm -lGLESv2 -lgbm -linput -ludev -lpthread -lX11 -lxkbcommon -lSOIL
INCLUDEPATH += /usr/include/drm
INCLUDEPATH += ../Wayland++/
INCLUDEPATH += ../Wayland++/classes/
INCLUDEPATH += ../Wayland++/globals/Wayland/
INCLUDEPATH += ../Wayland++/globals/XdgShell/
INCLUDEPATH += /usr/share/

SOURCES += \
        ../Wayland++/classes/WBackendDRM.cpp \
        ../Wayland++/classes/WBackendX11.cpp \
        ../Wayland++/classes/WClient.cpp \
        ../Wayland++/classes/WCompositor.cpp \
        ../Wayland++/classes/WInput.cpp \
        ../Wayland++/classes/WOpenGL.cpp \
        ../Wayland++/classes/WPositioner.cpp \
        ../Wayland++/classes/WRegion.cpp \
        ../Wayland++/classes/WSurface.cpp \
        ../Wayland++/classes/WTexture.cpp \
        ../Wayland++/classes/WView.cpp \
        ../Wayland++/classes/WWayland.cpp \
        ../Wayland++/globals/Wayland/Compositor.cpp \
        ../Wayland++/globals/Wayland/DataDevice.cpp \
        ../Wayland++/globals/Wayland/DataDeviceManager.cpp \
        ../Wayland++/globals/Wayland/Keyboard.cpp \
        ../Wayland++/globals/Wayland/Output.cpp \
        ../Wayland++/globals/Wayland/Pointer.cpp \
        ../Wayland++/globals/Wayland/Region.cpp \
        ../Wayland++/globals/Wayland/Seat.cpp \
        ../Wayland++/globals/Wayland/Subcompositor.cpp \
        ../Wayland++/globals/Wayland/Subsurface.cpp \
        ../Wayland++/globals/Wayland/Surface.cpp \
        ../Wayland++/globals/XdgShell/XdgPopup.cpp \
        ../Wayland++/globals/XdgShell/XdgPositioner.cpp \
        ../Wayland++/globals/XdgShell/XdgSurface.cpp \
        ../Wayland++/globals/XdgShell/XdgToplevel.cpp \
        ../Wayland++/globals/XdgShell/XdgWmBase.cpp \
        ../Wayland++/globals/XdgShell/xdg-shell.c \
        MyClient.cpp \
        MyCompositor.cpp \
        MySurface.cpp \
        main.cpp

HEADERS += \
    ../Wayland++/classes/WBackend.h \
    ../Wayland++/classes/WClient.h \
    ../Wayland++/classes/WCompositor.h \
    ../Wayland++/classes/WInput.h \
    ../Wayland++/classes/WNamespaces.h \
    ../Wayland++/classes/WOpenGL.h \
    ../Wayland++/classes/WPositioner.h \
    ../Wayland++/classes/WRegion.h \
    ../Wayland++/classes/WSurface.h \
    ../Wayland++/classes/WTexture.h \
    ../Wayland++/classes/WView.h \
    ../Wayland++/classes/WWayland.h \
    ../Wayland++/globals/Wayland/Compositor.h \
    ../Wayland++/globals/Wayland/DataDevice.h \
    ../Wayland++/globals/Wayland/DataDeviceManager.h \
    ../Wayland++/globals/Wayland/Keyboard.h \
    ../Wayland++/globals/Wayland/Output.h \
    ../Wayland++/globals/Wayland/Pointer.h \
    ../Wayland++/globals/Wayland/Region.h \
    ../Wayland++/globals/Wayland/Seat.h \
    ../Wayland++/globals/Wayland/Subcompositor.h \
    ../Wayland++/globals/Wayland/Subsurface.h \
    ../Wayland++/globals/Wayland/Surface.h \
    ../Wayland++/globals/XdgShell/XdgPopup.h \
    ../Wayland++/globals/XdgShell/XdgPositioner.h \
    ../Wayland++/globals/XdgShell/XdgSurface.h \
    ../Wayland++/globals/XdgShell/XdgToplevel.h \
    ../Wayland++/globals/XdgShell/XdgWmBase.h \
    ../Wayland++/globals/XdgShell/xdg-shell.h \
    MyClient.h \
    MyCompositor.h \
    MySurface.h

DISTFILES += \
    ../Wayland++/protocols/wayland.xml \
    ../Wayland++/protocols/xdg-shell-unstable-v6.xml \
    shaders/Fragment.glsl \
    shaders/Vertex.glsl

SUBDIRS += \
    ../Wayland++/Wayland++.pro

TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lwayland-server -lEGL -lGL -ldrm -lGLESv2 -lgbm -linput -ludev -lpthread -lX11 -lxkbcommon
INCLUDEPATH += /usr/include/drm
INCLUDEPATH += ../Wayland++/
INCLUDEPATH += /usr/share/

SOURCES += \
        ../Wayland++/WBackendDRM.cpp \
        ../Wayland++/WBackendX11.cpp \
        ../Wayland++/WClient.cpp \
        ../Wayland++/WCompositor.cpp \
        ../Wayland++/WInput.cpp \
        ../Wayland++/WOpenGL.cpp \
        ../Wayland++/WRegion.cpp \
        ../Wayland++/WSurface.cpp \
        ../Wayland++/WTexture.cpp \
        ../Wayland++/WView.cpp \
        ../Wayland++/WWayland.cpp \
        ../Wayland++/globals/Compositor.cpp \
        ../Wayland++/globals/DataDeviceManager.cpp \
        ../Wayland++/globals/Keyboard.cpp \
        ../Wayland++/globals/Output.cpp \
        ../Wayland++/globals/Pointer.cpp \
        ../Wayland++/globals/Region.cpp \
        ../Wayland++/globals/Seat.cpp \
        ../Wayland++/globals/Surface.cpp \
        ../Wayland++/globals/XdgPopup.cpp \
        ../Wayland++/globals/XdgSurface.cpp \
        ../Wayland++/globals/XdgToplevel.cpp \
        ../Wayland++/globals/XdgWmBase.cpp \
        ../Wayland++/protocols/xdg-shell.c \
        MyCompositor.cpp \
        main.cpp

HEADERS += \
    ../Wayland++/WBackend.h \
    ../Wayland++/WClient.h \
    ../Wayland++/WCompositor.h \
    ../Wayland++/WInput.h \
    ../Wayland++/WOpenGL.h \
    ../Wayland++/WRegion.h \
    ../Wayland++/WSurface.h \
    ../Wayland++/WTexture.h \
    ../Wayland++/WTypes.h \
    ../Wayland++/WView.h \
    ../Wayland++/WWayland.h \
    ../Wayland++/globals/Compositor.h \
    ../Wayland++/globals/DataDeviceManager.h \
    ../Wayland++/globals/Keyboard.h \
    ../Wayland++/globals/Output.h \
    ../Wayland++/globals/Pointer.h \
    ../Wayland++/globals/Region.h \
    ../Wayland++/globals/Seat.h \
    ../Wayland++/globals/Surface.h \
    ../Wayland++/globals/XdgPopup.h \
    ../Wayland++/globals/XdgSurface.h \
    ../Wayland++/globals/XdgToplevel.h \
    ../Wayland++/globals/XdgWmBase.h \
    ../Wayland++/protocols/xdg-shell.h \
    MyCompositor.h

DISTFILES += \
    shaders/Fragment.glsl \
    shaders/Vertex.glsl

TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lwayland-server -lEGL -lGL -ldrm -lGLESv2 -lgbm -linput -ludev -lpthread -lX11
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
    ../Wayland++/WView.h \
    ../Wayland++/WWayland.h \
    ../Wayland++/protocols/xdg-shell.h \
    MyCompositor.h

DISTFILES += \
    shaders/Fragment.glsl \
    shaders/Vertex.glsl

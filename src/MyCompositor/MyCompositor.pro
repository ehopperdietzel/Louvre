TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lwayland-server -lEGL -lGL -ldrm -lGLESv2 -lgbm -linput -ludev -lpthread
INCLUDEPATH += /usr/include/drm
INCLUDEPATH += ../Wayland++/

SOURCES += \
        ../Wayland++/WBackendDRM.cpp \
        ../Wayland++/WClient.cpp \
        ../Wayland++/WCompositor.cpp \
        ../Wayland++/WInput.cpp \
        ../Wayland++/WOpenGL.cpp \
        ../Wayland++/WRegion.cpp \
        ../Wayland++/WSurface.cpp \
        ../Wayland++/WTexture.cpp \
        ../Wayland++/WView.cpp \
        ../Wayland++/WWayland.cpp \
        MyCompositor.cpp \
        main.cpp

HEADERS += \
    ../Wayland++/WBackendDRM.h \
    ../Wayland++/WClient.h \
    ../Wayland++/WCompositor.h \
    ../Wayland++/WInput.h \
    ../Wayland++/WOpenGL.h \
    ../Wayland++/WRegion.h \
    ../Wayland++/WSurface.h \
    ../Wayland++/WTexture.h \
    ../Wayland++/WView.h \
    ../Wayland++/WWayland.h \
    MyCompositor.h

DISTFILES += \
    shaders/Fragment.glsl \
    shaders/Vertex.glsl

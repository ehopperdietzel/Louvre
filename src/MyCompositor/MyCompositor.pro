TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lwayland-server -lEGL -lGL -ldrm -lGLESv2 -lgbm -linput -ludev -lpthread
INCLUDEPATH += /usr/include/drm
INCLUDEPATH += ../Wayland++/

SOURCES += \
        ../Wayland++/WBackendDRM.cpp \
        ../Wayland++/WCompositor.cpp \
        ../Wayland++/WInput.cpp \
        ../Wayland++/WView.cpp \
        MyCompositor.cpp \
        main.cpp

HEADERS += \
    ../Wayland++/WBackendDRM.h \
    ../Wayland++/WCompositor.h \
    ../Wayland++/WInput.h \
    ../Wayland++/WView.h \
    MyCompositor.h

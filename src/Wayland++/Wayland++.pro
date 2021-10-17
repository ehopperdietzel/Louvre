TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

LIBS += -lwayland-server -lEGL -lGL -ldrm -lGLESv2 -lgbm -linput -ludev -lpthread
INCLUDEPATH += /usr/include/drm

SOURCES += \
        Crystals.cpp \
        WBackendDRM.cpp \
        WCompositor.cpp \
        WInput.cpp \
        WView.cpp \
        main.cpp

HEADERS += \
    Crystals.h \
    WBackendDRM.h \
    WCompositor.h \
    WInput.h \
    WView.h

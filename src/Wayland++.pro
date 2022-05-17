TEMPLATE = lib
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
TARGET = Wpp
DESTDIR = $$PWD/build

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2

# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O3

LIBS += -L/usr/local/lib -lwayland-server -lEGL -lGL -ldrm -lGLESv2 -lgbm -linput -ludev -lpthread -lX11 -lXfixes -lxkbcommon -lSOIL
INCLUDEPATH += /usr/include/drm ./classes ./globals/Wayland ./globals/XdgShell /usr/local/include

HEADERS += \
    classes/WBackend.h \
    classes/WClient.h \
    classes/WCompositor.h \
    classes/WCursor.h \
    classes/WKeyboard.h \
    classes/WNamespaces.h \
    classes/WOpenGL.h \
    classes/WOutput.h \
    classes/WOutputManager.h \
    classes/WOutputMode.h \
    classes/WPoint.h \
    classes/WPointF.h \
    classes/WPointer.h \
    classes/WPositioner.h \
    classes/WRect.h \
    classes/WRegion.h \
    classes/WSeat.h \
    classes/WSize.h \
    classes/WSizeF.h \
    classes/WSurface.h \
    classes/WTexture.h \
    classes/WTime.h \
    classes/WView.h \
    classes/WWayland.h \
    globals/Viewporter/Viewporter.h \
    globals/Viewporter/viewporter.h \
    globals/Wayland/Compositor.h \
    globals/Wayland/DataDevice.h \
    globals/Wayland/DataDeviceManager.h \
    globals/Wayland/Keyboard.h \
    globals/Wayland/Output.h \
    globals/Wayland/Pointer.h \
    globals/Wayland/Region.h \
    globals/Wayland/Seat.h \
    globals/Wayland/Subcompositor.h \
    globals/Wayland/Subsurface.h \
    globals/Wayland/Surface.h \
    globals/XdgShell/XdgPopup.h \
    globals/XdgShell/XdgPositioner.h \
    globals/XdgShell/XdgSurface.h \
    globals/XdgShell/XdgToplevel.h \
    globals/XdgShell/XdgWmBase.h \
    globals/XdgShell/xdg-shell.h

SOURCES += \
    classes/WBackendDRM.cpp \
    classes/WBackendX11.cpp \
    classes/WClient.cpp \
    classes/WCompositor.cpp \
    classes/WCursor.cpp \
    classes/WKeyboard.cpp \
    classes/WOpenGL.cpp \
    classes/WOutput.cpp \
    classes/WOutputManager.cpp \
    classes/WOutputMode.cpp \
    classes/WPoint.cpp \
    classes/WPointF.cpp \
    classes/WPointer.cpp \
    classes/WPositioner.cpp \
    classes/WRegion.cpp \
    classes/WSeat.cpp \
    classes/WSurface.cpp \
    classes/WTexture.cpp \
    classes/WTime.cpp \
    classes/WView.cpp \
    classes/WWayland.cpp \
    globals/Viewporter/Viewporter.cpp \
    globals/Viewporter/viewporter.c \
    globals/Wayland/Compositor.cpp \
    globals/Wayland/DataDevice.cpp \
    globals/Wayland/DataDeviceManager.cpp \
    globals/Wayland/Keyboard.cpp \
    globals/Wayland/Output.cpp \
    globals/Wayland/Pointer.cpp \
    globals/Wayland/Region.cpp \
    globals/Wayland/Seat.cpp \
    globals/Wayland/Subcompositor.cpp \
    globals/Wayland/Subsurface.cpp \
    globals/Wayland/Surface.cpp \
    globals/XdgShell/XdgPopup.cpp \
    globals/XdgShell/XdgPositioner.cpp \
    globals/XdgShell/XdgSurface.cpp \
    globals/XdgShell/XdgToplevel.cpp \
    globals/XdgShell/XdgWmBase.cpp \
    globals/XdgShell/xdg-shell.c


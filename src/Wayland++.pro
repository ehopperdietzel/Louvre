TEMPLATE = lib
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt
TARGET = Wpp
DESTDIR = $$PWD/build

LIBS += -L/usr/local/lib -lwayland-server -lEGL -lGL -ldrm -lGLESv2 -lgbm -linput -ludev -lpthread -lX11 -lXfixes -lxkbcommon
INCLUDEPATH += /usr/include/drm ./classes ./globals/Wayland ./globals/XdgShell /usr/local/include

HEADERS += \
    classes/WBackend.h \
    classes/WClient.h \
    classes/WCompositor.h \
    classes/WInput.h \
    classes/WNamespaces.h \
    classes/WOpenGL.h \
    classes/WOutput.h \
    classes/WOutputManager.h \
    classes/WPoint.h \
    classes/WPointF.h \
    classes/WPositioner.h \
    classes/WRect.h \
    classes/WRegion.h \
    classes/WSize.h \
    classes/WSurface.h \
    classes/WTexture.h \
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
    classes/WInput.cpp \
    classes/WOpenGL.cpp \
    classes/WOutput.cpp \
    classes/WOutputManager.cpp \
    classes/WPoint.cpp \
    classes/WPointF.cpp \
    classes/WPositioner.cpp \
    classes/WRegion.cpp \
    classes/WSurface.cpp \
    classes/WTexture.cpp \
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


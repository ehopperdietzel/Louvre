TEMPLATE = lib

CONFIG -= qt
CONFIG += c++17

DEFINES += Desk

DESTDIR = $$PWD/build

#QMAKE_CXXFLAGS_RELEASE -= -O
#QMAKE_CXXFLAGS_RELEASE -= -O1
#QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

LIBS += -L/usr/local/lib -lwayland-server -lEGL -lGL -ldrm -lGLESv2 -lgbm -linput -ludev -lpthread -lX11 -lXfixes -lXcursor -lxkbcommon -lSOIL
INCLUDEPATH += /usr/include/drm ./classes ./globals/Wayland ./globals/XdgShell /usr/local/include


# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    classes/LBackend.h \
    classes/LClient.h \
    classes/LCompositor.h \
    classes/LCursor.h \
    classes/LNamespaces.h \
    classes/LOpenGL.h \
    classes/LOutput.h \
    classes/LOutputManager.h \
    classes/LOutputMode.h \
    classes/LPoint.h \
    classes/LPointF.h \
    classes/LPopup.h \
    classes/LPositioner.h \
    classes/LRect.h \
    classes/LRegion.h \
    classes/LSeat.h \
    classes/LSize.h \
    classes/LSizeF.h \
    classes/LSurface.h \
    classes/LTexture.h \
    classes/LTime.h \
    classes/LToplevel.h \
    classes/LWayland.h \
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
    classes/LBackendDRM.cpp \
    classes/LBackendX11.cpp \
    classes/LClient.cpp \
    classes/LCompositor.cpp \
    classes/LCursor.cpp \
    classes/LOpenGL.cpp \
    classes/LOutput.cpp \
    classes/LOutputManager.cpp \
    classes/LOutputMode.cpp \
    classes/LPoint.cpp \
    classes/LPointF.cpp \
    classes/LPopup.cpp \
    classes/LPositioner.cpp \
    classes/LRegion.cpp \
    classes/LSeat.cpp \
    classes/LSurface.cpp \
    classes/LTexture.cpp \
    classes/LTime.cpp \
    classes/LToplevel.cpp \
    classes/LWayland.cpp \
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

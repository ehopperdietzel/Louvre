TEMPLATE = lib

CONFIG -= qt
CONFIG += c++17

DEFINES += Desk

DESTDIR = $$PWD/../../build

QMAKE_CXXFLAGS_DEBUG *= -O
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE *= -O3

LIBS += -L/usr/local/lib/x86_64-linux-gnu -lwayland-server -lEGL -lGL -lGLESv2 -linput -ludev -lpthread -lXcursor -lxkbcommon -lSOIL
INCLUDEPATH += /usr/include/libdrm ./classes ./classes/private ./globals/Wayland ./globals/XdgShell /usr/local/include


# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    classes/LBaseSurfaceRole.h \
    classes/LClient.h \
    classes/LCompositor.h \
    classes/LCursor.h \
    classes/LCursorRole.h \
    classes/LDataDevice.h \
    classes/LDataOffer.h \
    classes/LDataSource.h \
    classes/LKeyboard.h \
    classes/LNamespaces.h \
    classes/LOpenGL.h \
    classes/LOutput.h \
    classes/LOutputManager.h \
    classes/LOutputMode.h \
    classes/LPoint.h \
    classes/LPointF.h \
    classes/LPointer.h \
    classes/LPopupRole.h \
    classes/LPositioner.h \
    classes/LRect.h \
    classes/LRegion.h \
    classes/LSeat.h \
    classes/LSize.h \
    classes/LSizeF.h \
    classes/LSubsurfaceRole.h \
    classes/LSurface.h \
    classes/LTexture.h \
    classes/LTime.h \
    classes/LToplevelRole.h \
    classes/LWayland.h \
    classes/private/LClientPrivate.h \
    classes/private/LCompositorPrivate.h \
    classes/private/LDataDevicePrivate.h \
    classes/private/LDataOfferPrivate.h \
    classes/private/LDataSourcePrivate.h \
    classes/private/LOutputManagerPrivate.h \
    classes/private/LOutputModePrivate.h \
    classes/private/LOutputPrivate.h \
    classes/private/LSurfacePrivate.h \
    globals/Viewporter/Viewporter.h \
    globals/Viewporter/viewporter.h \
    globals/Wayland/Compositor.h \
    globals/Wayland/DataDevice.h \
    globals/Wayland/DataDeviceManager.h \
    globals/Wayland/DataOffer.h \
    globals/Wayland/DataSource.h \
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
    classes/LBaseSurfaceRole.cpp \
    classes/LClient.cpp \
    classes/LCompositor.cpp \
    classes/LCursor.cpp \
    classes/LCursorRole.cpp \
    classes/LDataDevice.cpp \
    classes/LDataOffer.cpp \
    classes/LDataSource.cpp \
    classes/LKeyboard.cpp \
    classes/LOpenGL.cpp \
    classes/LOutput.cpp \
    classes/LOutputManager.cpp \
    classes/LOutputMode.cpp \
    classes/LPoint.cpp \
    classes/LPointF.cpp \
    classes/LPointer.cpp \
    classes/LPopupRole.cpp \
    classes/LPositioner.cpp \
    classes/LRegion.cpp \
    classes/LSeat.cpp \
    classes/LSubsurfaceRole.cpp \
    classes/LSurface.cpp \
    classes/LTexture.cpp \
    classes/LTime.cpp \
    classes/LToplevelRole.cpp \
    classes/LWayland.cpp \
    globals/Viewporter/Viewporter.cpp \
    globals/Viewporter/viewporter.c \
    globals/Wayland/Compositor.cpp \
    globals/Wayland/DataDevice.cpp \
    globals/Wayland/DataDeviceManager.cpp \
    globals/Wayland/DataOffer.cpp \
    globals/Wayland/DataSource.cpp \
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

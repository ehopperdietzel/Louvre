#ifndef LNAMESPACES_H
#define LNAMESPACES_H

#include <libinput.h>
#include <xdg-shell.h>
/*
#include <Wayland-5.h>
#include <wayland-server-core.h>
#include <wayland-server-protocol.h>
#include <wayland-server.h>
#include <wayland-util.h>
#include <wayland-version.h>
#include <wayland-egl-core.h>
#include <wayland-egl.h>
*/

#define LOUVRE_TB_H 80
#define LOUBRE_DEBUG 1

// Globals versions

#define LOUVRE_XDG_WM_BASE_VERSION 2

#define LOUVRE_SEAT_VERSION 7
#define LOUVRE_SUBCOMPOSITOR_VERSION 1
#define LOUVRE_XDG_SHELL_VERSION 2


namespace Louvre
{
    // API Classes
    class LBackend;
    class LClient;
    class LCompositor;
    class LOutput;
    class LOutputManager;
    class LOutputMode;
    class LOpenGL;
    class LPositioner;
    class LRegion;
    class LSurface;
    class LTexture;
    class LWayland;

    // Surface roles
    class LToplevelRole;
    class LPopupRole;
    class LSubsurfaceRole;

    // Input related
    class LSeat;
    class LCursor;

    // Utils
    class LTime;
    class LPoint;
    class LPointF;
    class LRect;

    // Types
    typedef uint32_t        UInt32;
    typedef int32_t         Int32;
    typedef uint64_t        UInt64;
    typedef int64_t         Int64;
    typedef float           Float32;
    typedef double          Float64;
    typedef unsigned char   UChar8;
    typedef LPoint LSize;
    typedef LPointF LSizeF;
    typedef UInt32 LKey;

    typedef void* EGLContext;
    typedef void* EGLDisplay;

    // Wayland Globals
    namespace Globals
    {
        class Compositor;
        class Subcompositor;
        class DataDeviceManager;
        class DataDevice;
        class DataSource;
        class Keyboard;
        class Output;
        class Pointer;
        class Region;
        class Seat;
        class Surface;
        class Subsurface;
    };

    // Extensions
    namespace Extensions
    {
        namespace XdgShell
        {
            class Popup;
            class Surface;
            class Toplevel;
            class WmBase;
            class Positioner;
        };
        namespace Viewporter
        {
            class Viewporter;
            class Viewport;
        };
    };

};

#endif // LNAMESPACES_H

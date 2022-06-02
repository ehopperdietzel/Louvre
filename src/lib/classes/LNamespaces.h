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
#define LOUBRE_DEBUG 0

// Globals versions

#define LOUVRE_XDG_WM_BASE_VERSION 2

#define LOUVRE_SEAT_VERSION 7
#define LOUVRE_XDG_SHELL_VERSION 2

#ifndef DEFINE_ENUM_FLAG_OPERATORS
#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) \
inline ENUMTYPE operator | (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) | ((int)b)); } \
inline ENUMTYPE &operator |= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) |= ((int)b)); } \
inline ENUMTYPE operator & (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) & ((int)b)); } \
inline ENUMTYPE &operator &= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) &= ((int)b)); } \
inline ENUMTYPE operator ~ (ENUMTYPE a) { return ENUMTYPE(~((int)a)); } \
inline ENUMTYPE operator ^ (ENUMTYPE a, ENUMTYPE b) { return ENUMTYPE(((int)a) ^ ((int)b)); } \
inline ENUMTYPE &operator ^= (ENUMTYPE &a, ENUMTYPE b) { return (ENUMTYPE &)(((int &)a) ^= ((int)b)); }
#else
#define DEFINE_ENUM_FLAG_OPERATORS(ENUMTYPE) // NOP, C allows these operators.
#endif


namespace Louvre
{
    // API Classes
    class LBackend;
    class LClient;
    class LCompositor;
    class LOutput;
    class LOutputManager;
    class LOpenGL;
    class LPositioner;
    class LRegion;
    class LSurface;
    class LTexture;
    class LWayland;

    // Surface roles
    class LToplevelRole;
    class LPopupRole;

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

    struct Point
    {
        Int32 x = 0;
        Int32 y = 0;
    };

    struct PointD
    {
        double x = 0;
        double y = 0;
    };

    struct Size
    {
        Int32 width = 0;
        Int32 height = 0;
    };

    struct Rect
    {
        Int32 x = 0;
        Int32 y = 0;
        Int32 width = 0;
        Int32 height = 0;
    };

    struct RectD
    {
        double x = 0;
        double y = 0;
        double width = 0;
        double height = 0;
    };


    enum POINTER_BUTTON : UInt32
    {
        LEFT_BUTTON = 272
    };


    enum class LToplevelStateFlags : UChar8
    {
        Deactivated    = 0,
        Maximized      = 1,
        Fullscreen     = 2,
        Resizing       = 4,
        Activated      = 8,

    #if LOUVRE_XDG_WM_BASE_VERSION >= 2
        TiledLeft      = 16,
        TiledRight     = 32,
        TiledTop       = 64,
        TiledBottom    = 128
    #endif

    };

    DEFINE_ENUM_FLAG_OPERATORS(LToplevelStateFlags)



    // Wayland Globals
    namespace Globals
    {
        class Compositor;
        class Subcompositor;
        class DataDeviceManager;
        class DataDevice;
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

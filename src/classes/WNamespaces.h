#ifndef WNAMESPACES_H
#define WNAMESPACES_H

#include <libinput.h>
#include <xdg-shell.h>

#include <wayland-server-core.h>
#include <wayland-server-protocol.h>
#include <wayland-server.h>
#include <wayland-util.h>
#include <wayland-version.h>
#include <wayland-egl-core.h>
#include <wayland-egl.h>

#define W_WIDTH 2880
#define W_HEIGHT 1800

namespace Wpp
{
    // API Classes
    class WBackend;
    class WClient;
    class WCompositor;
    class WInput;
    class WOutput;
    class WOutputManager;
    class WOpenGL;
    class WPositioner;
    class WRegion;
    class WSurface;
    class WTexture;
    class WTypes;
    class WView;
    class WWayland;

    class WPoint;
    class WPointF;
    class WRect;

    // Types
    typedef uint32_t        UInt32;
    typedef int32_t         Int32;
    typedef uint64_t        UInt64;
    typedef int64_t         Int64;
    typedef unsigned char   SurfaceStateFlags;
    typedef xdg_positioner_anchor Anchor;
    typedef xdg_positioner_gravity Gravity;
    typedef xdg_positioner_constraint_adjustment ConstraintAdjustment;
    typedef WPoint WSize;

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

    struct WRegionRect
    {
        Int32 x = 0;
        Int32 y = 0;
        Int32 width = 0;
        Int32 height = 0;
        bool add = true;
    };

    enum ResizeEdge : UInt32
    {
        Top = 1,
        Bottom = 2,
        Left = 4,
        TopLeft = 5,
        BottomLeft = 6,
        Right = 8,
        TopRight = 9,
        BottomRight = 10
    };

    enum SurfaceState : unsigned char
    {
        Maximized = 1, // 1
        Fullscreen = 2, // 2
        Resizing = 4, // 3
        Activated = 8, // 4
        TiledLeft = 16, // 5
        TiledRight = 32, // 6
        TiledTop = 64, // 7
        TiledBottom = 128 // 8
    };


    enum SurfaceType : UInt32
    {
        Undefined = 0,
        Toplevel = 1,
        Popup = 2,
        Subsurface = 3,
        Cursor = 4
    };


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
    };

};
#endif // WNAMESPACES_H

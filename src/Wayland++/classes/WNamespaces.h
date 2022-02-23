#ifndef WNAMESPACES_H
#define WNAMESPACES_H

#include <libinput.h>

namespace WaylandPlus
{
    // API Classes
    class WBackend;
    class WClient;
    class WCompositor;
    class WInput;
    class WOpenGL;
    class WRegion;
    class WSurface;
    class WTexture;
    class WTypes;
    class WView;
    class WWayland;

    // Types
    typedef uint32_t    UInt32;
    typedef int32_t     Int32;

    struct Rect
    {
        Int32 x = 0;
        Int32 y = 0;
        Int32 width = 0;
        Int32 height = 0;
    };

    struct WRegionRect
    {
        Int32 x = 0;
        Int32 y = 0;
        Int32 width = 0;
        Int32 height = 0;
        bool add = true;
    };

    // Wayland Globals
    namespace Globals
    {
        class Compositor;
        class DataDeviceManager;
        class Keyboard;
        class Output;
        class Pointer;
        class Region;
        class Seat;
        class Surface;
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
        };
    };

};
#endif // WNAMESPACES_H

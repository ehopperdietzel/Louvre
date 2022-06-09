#include "LCursorRole.h"
#include <LSurface.h>

using namespace Louvre;

LCursorRole::LCursorRole(wl_resource *resource, LSurface *surface) : LBaseSurfaceRole(resource, surface)
{
    p_roleId = LSurface::Cursor;
}

LCursorRole::~LCursorRole()
{

}

const LPoint &LCursorRole::rolePos() const
{
    p_rolePos = surface()->pos() - hotspot();
    return p_rolePos;
}

const LPoint &Louvre::LCursorRole::hotspot() const
{
    return p_hotspot;
}

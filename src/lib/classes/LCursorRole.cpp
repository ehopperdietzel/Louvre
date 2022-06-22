#include "LCursorRole.h"
#include <LSurface.h>

using namespace Louvre;

LCursorRole::LCursorRole(wl_resource *resource, LSurface *surface) : LBaseSurfaceRole(resource, surface)
{
    m_roleId = LSurface::Cursor;
}

LCursorRole::~LCursorRole()
{

}

const LPoint &LCursorRole::rolePos() const
{
    m_rolePos = surface()->pos() - hotspot();
    return m_rolePos;
}

const LPoint &Louvre::LCursorRole::hotspot() const
{
    return m_hotspot;
}

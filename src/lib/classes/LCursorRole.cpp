#include "LCursorRole.h"
#include <LSurface.h>
#include <LBaseSurfaceRolePrivate.h>

using namespace Louvre;

LCursorRole::LCursorRole(wl_resource *resource, LSurface *surface) : LBaseSurfaceRole(resource, surface)
{
    baseImp()->roleId = LSurface::Cursor;
}

LCursorRole::~LCursorRole()
{

}

const LPoint &LCursorRole::rolePos() const
{
   baseImp()->rolePos = surface()->pos() - hotspot();
   return baseImp()->rolePos;
}

const LPoint &Louvre::LCursorRole::hotspot() const
{
    return m_hotspot;
}

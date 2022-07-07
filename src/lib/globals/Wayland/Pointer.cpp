#include "Pointer.h"
#include <stdio.h>
#include <LCompositor.h>
#include <LSeat.h>
#include <LPoint.h>
#include <LCursorRole.h>
#include <LPointer.h>

#include <LClientPrivate.h>
#include <LSurfacePrivate.h>

using namespace Louvre::Globals;

void Pointer::resource_destroy(wl_resource *resource)
{
    printf("POINTER DESTROYED.\n");
    LClient *client = (LClient*)wl_resource_get_user_data(resource);
    client->imp()->m_pointerResource = nullptr;
}

void Pointer::set_cursor(wl_client *, wl_resource *resource, UInt32 serial, wl_resource *surface, Int32 hotspot_x, Int32 hotspot_y)
{
    LClient *lClient = (LClient*)wl_resource_get_user_data(resource);

    if(serial != lClient->seat()->pointer()->lastPointerEnterEventSerial())
        return;

    if(surface)
    {
        LSurface *lSurface = (LSurface*)wl_resource_get_user_data(surface);

        if(lSurface->roleType() != LSurface::Undefined && lSurface->roleType() != LSurface::Cursor)
        {
            printf("CURSOR ERROR.\n");
            wl_resource_post_error(resource,WL_POINTER_ERROR_ROLE,"Given wl_surface has another role.");
            return;
        }

        LCursorRole *lCursor = new LCursorRole(lSurface->resource(),lSurface);
        lCursor->m_hotspot = LPoint(hotspot_x,hotspot_y)*lSurface->bufferScale();
        lSurface->imp()->m_role = lCursor;
        lSurface->roleChanged();
        lClient->compositor()->seat()->pointer()->setCursorRequest(lSurface,lCursor->m_hotspot.x(),lCursor->m_hotspot.y());
    }
    else
    {
        lClient->seat()->pointer()->setCursorRequest(nullptr,0,0);
    }
}

#if LOUVRE_SEAT_VERSION >= 3
void Pointer::release(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}
#endif

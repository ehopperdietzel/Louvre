#include <WPointer.h>
#include <WSurface.h>
#include <WClient.h>
#include <WWayland.h>
#include <WTime.h>
#include <WCompositor.h>
#include <WSeat.h>

using namespace Wpp;

Wpp::WPointer::WPointer(WSurface *surface)
{
    p_surface = surface;
}

void WPointer::sendButtonEvent(UInt32 buttonCode, UInt32 buttonState) const
{
    if(!p_hasResource()) return;

    wl_pointer_send_button(p_resource,WWayland::nextSerial(),WTime::ms(),buttonCode,buttonState);

    if(wl_resource_get_version(p_resource) >= 5)
        wl_pointer_send_frame(p_resource);

}

void WPointer::sendMotionEvent(Float64 x, Float64 y) const
{
    if(!p_hasResource()) return;

    wl_pointer_send_motion(p_resource,WTime::ms(),wl_fixed_from_double(x),wl_fixed_from_double(y));

    if(wl_resource_get_version(p_resource) >= 5)
        wl_pointer_send_frame(p_resource);
}

void WPointer::setFocus(Float64 x, Float64 y) const
{
    // If already has focus
    if(hasFocus()) return;

    // If another surface has focus
    if(seat()->pointerFocus())
        seat()->pointerFocus()->pointer().unsetFocus();

    // If its not initialized no surface gets focus
    if(!p_hasResource())
    {
        seat()->p_pointerFocusSurface = nullptr;
        return;
    }

    seat()->p_pointerFocusSurface = p_surface;

    // Send focus event
    wl_pointer_send_enter(p_resource,WWayland::nextSerial(),p_surface->p_resource,wl_fixed_from_double(x),wl_fixed_from_double(y));
    if(wl_resource_get_version(p_resource) >= 5)
        wl_pointer_send_frame(p_resource);
}

void WPointer::unsetFocus() const
{
    // If already hasn't focus
    if(!hasFocus()) return;

    // If has a wl_pointer resource
    if(p_hasResource())
    {
        // Send the unset focus event
        wl_pointer_send_leave(p_resource,WWayland::nextSerial(),p_surface->p_resource);
        if(wl_resource_get_version(p_resource) >= 5)
            wl_pointer_send_frame(p_resource);
    }

    // No surface has focus
    seat()->p_pointerFocusSurface = nullptr;
}

bool WPointer::hasFocus() const
{
    return seat()->pointerFocus() == p_surface;
}

WSeat *WPointer::seat() const
{
    return p_surface->compositor()->seat();
}

bool WPointer::p_hasResource() const
{
    return ((p_resource = p_surface->p_client->p_pointerResource) != nullptr);
}




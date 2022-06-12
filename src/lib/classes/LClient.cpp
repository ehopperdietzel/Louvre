#include "LClient.h"
#include <LCompositor.h>

using namespace Louvre;

LClient::LClient(wl_client *client, LCompositor *compositor)
{
    p_client = client;
    p_compositor = compositor;
}

LClient::~LClient()
{
    printf("CLIENT DESTROYED.\n");
}

LCompositor *LClient::compositor() const
{
    return p_compositor;
}

LSeat *LClient::seat() const
{
    return p_compositor->seat();
}

wl_client *LClient::client() const
{
    return p_client;
}

UInt32 LClient::id() const
{
    return p_id;
}

const list<LSurface *> &LClient::surfaces() const
{
    return p_surfaces;
}

wl_resource *LClient::seatResource() const
{
    return p_seatResource;
}

wl_resource *LClient::pointerResource() const
{
    return p_pointerResource;
}

wl_resource *LClient::keyboardResource() const
{
    return p_keyboardResource;
}

wl_resource *LClient::touchResource() const
{
    return p_touchResource;
}

UInt32 LClient::lastPointerEnterEventSerial() const
{
    return p_lastPointerEnterEventSerial;
}

UInt32 LClient::lastPointerLeaveEventSerial() const
{
    return p_lastPointerLeaveEventSerial;
}

UInt32 LClient::lastPointerButtonEventSerial() const
{
    return p_lastPointerButtonEventSerial;
}

UInt32 LClient::lastKeyboardEnterEventSerial() const
{
    return p_lastKeyboardEnterEventSerial;
}

UInt32 LClient::lastKeyboardLeaveEventSerial() const
{
    return p_lastKeyboardLeaveEventSerial;
}

UInt32 LClient::lastKeyboardKeyEventSerial() const
{
    return p_lastKeyboardKeyEventSerial;
}

UInt32 LClient::lastKeyboardModifiersEventSerial() const
{
    return p_lastKeyboardModifiersEventSerial;
}

wl_resource *LClient::xdgWmBaseResource() const
{
    return p_xdgWmBaseResource;
}


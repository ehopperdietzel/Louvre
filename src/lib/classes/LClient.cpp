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
    printf("Client removed.\n");
}

LCompositor *LClient::compositor() const
{
    return p_compositor;
}

wl_client *LClient::client() const
{
    return p_client;
}

UInt32 LClient::id() const
{
    return p_id;
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


#include "WClient.h"
#include <WCompositor.h>

using namespace Wpp;

WClient::WClient(wl_client *client, WCompositor *compositor)
{
    p_client = client;
    p_compositor = compositor;
}

WClient::~WClient()
{
    printf("Client removed.\n");
}

WCompositor *WClient::compositor() const
{
    return p_compositor;
}

wl_client *WClient::client() const
{
    return p_client;
}

UInt32 WClient::id() const
{
    return p_id;
}

wl_resource *WClient::seatResource() const
{
    return p_seatResource;
}

wl_resource *WClient::pointerResource() const
{
    return p_pointerResource;
}

wl_resource *WClient::keyboardResource() const
{
    return p_keyboardResource;
}

wl_resource *WClient::touchResource() const
{
    return p_touchResource;
}


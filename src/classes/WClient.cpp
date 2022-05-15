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

WCompositor *WClient::compositor()
{
    return p_compositor;
}

wl_client *WClient::client()
{
    return p_client;
}

UInt32 WClient::id()
{
    return p_id;
}


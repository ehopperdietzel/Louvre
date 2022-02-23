#include "WClient.h"
#include <WCompositor.h>

using namespace WaylandPlus;

WClient::WClient(wl_client *client, wl_resource *resource, WCompositor *compositor)
{
    _client = client;
    _resource = resource;
    _compositor = compositor;

    // Append client to compositor
    _compositor->clients.push_back(this);

    // Trigger new client notification
    _compositor->newClient(this);
}

WClient::~WClient()
{

}

WCompositor *WClient::getCompositor()
{
    return _compositor;
}

wl_client *WClient::getClient()
{
    return _client;
}

wl_resource *WClient::getResource()
{
    return _resource;
}

wl_resource *WClient::getKeyboard()
{
    return _keyboard;
}

wl_resource *WClient::getPointer()
{
    return _pointer;
}

void WClient::newSurface(WSurface *)
{

}

void WClient::surfaceDestroyed(WSurface *)
{

}

void WClient::newRegion(WRegion *)
{

}

void WClient::regionDestroyed(WRegion *)
{

}

void WClient::setPointer(wl_resource *pointer)
{
    _pointer = pointer;
}

void WClient::setKeyboard(wl_resource *keyboard)
{
    _keyboard = keyboard;
}

UInt32 WClient::getId()
{
    return _id;
}

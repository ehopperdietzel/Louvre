#include "WClient.h"
#include <WCompositor.h>

using namespace WaylandPlus;

WClient::WClient(wl_client *client, WCompositor *compositor)
{
    _client = client;
    _compositor = compositor;

    // Append client to compositor
    _compositor->clients.push_back(this);
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

wl_resource *WClient::getKeyboard()
{
    return _keyboard;
}

wl_resource *WClient::getPointer()
{
    return _pointer;
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

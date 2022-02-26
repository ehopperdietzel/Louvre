#include "WPositioner.h"
#include <WClient.h>
#include <WCompositor.h>

using namespace WaylandPlus;

WPositioner::WPositioner(WClient *client)
{
    _client = client;

    _client->positioners.push_back(this);
    _client->newPositioner(this);
    _client->getCompositor()->newPositioner(this);
}

WClient *WPositioner::getClient()
{
    return _client;
}

wl_resource *WPositioner::getResource()
{
    return _resource;
}

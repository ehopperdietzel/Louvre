#include "WPositioner.h"
#include <WClient.h>
#include <WCompositor.h>

using namespace Wpp;

WPositioner::WPositioner(WClient *client)
{
    _client = client;
    _client->positioners.push_back(this);
    //_client->newPositioner(this);
}


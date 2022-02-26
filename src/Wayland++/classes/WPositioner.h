#ifndef WPOSITIONER_H
#define WPOSITIONER_H

#include <WNamespaces.h>

class WaylandPlus::WPositioner
{
public:
    WPositioner(WClient *client);

    WClient *getClient();
    wl_resource *getResource();
private:
    friend class WaylandPlus::Extensions::XdgShell::WmBase;
    wl_resource *_resource = nullptr;
    WClient *_client = nullptr;
};

#endif // WPOSITIONER_H

#ifndef WCLIENT_H
#define WCLIENT_H

#include <list>
#include <wayland-server.h>
#include <WRegion.h>
#include <WSurface.h>

using namespace std;

class WaylandPlus::WClient
{
public:
    WClient(wl_client *client, wl_resource *resource, WCompositor *compositor);
    virtual ~WClient();

    WCompositor *getCompositor();
    wl_client *getClient();
    wl_resource *getResource();
    wl_resource *getKeyboard();
    wl_resource *getPointer();

    virtual void newSurface(WSurface *surface);
    virtual void surfaceDestroyed(WSurface *surface);
    virtual void newRegion(WRegion *region);
    virtual void regionDestroyed(WRegion *region);

    void setPointer(wl_resource *pointer);
    void setKeyboard(wl_resource *keyboard);

    list<WRegion*>regions;
    list<WSurface*>surfaces;

    UInt32 getId();
private:
    WCompositor *_compositor = nullptr;
    wl_client *_client = nullptr;
    wl_resource *_resource = nullptr;
    wl_resource *_keyboard = nullptr;
    wl_resource *_pointer = nullptr;
    UInt32 _id;
};

#endif // WCLIENT_H

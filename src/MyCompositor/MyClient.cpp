#include "MyClient.h"
#include <MySurface.h>
#include <MyCompositor.h>

MyClient::MyClient(wl_client *client, WCompositor *compositor) : WClient::WClient(client,compositor){}

WSurface *MyClient::newSurfaceRequest(UInt32 id, wl_resource *resource)
{
    MyCompositor *comp = (MyCompositor*)getCompositor();
    MySurface *surface = new MySurface(id,resource,this);
    comp->surfacesList.push_back(surface);

    /* Initially all surfaces have undefined type (surface->getType()).
     * You should wait for the WSurface::typeChangeRequest before drawing it.*/

    return surface;
}

void MyClient::surfaceDestroyRequest(WSurface *surface)
{
    MyCompositor *comp = (MyCompositor*)getCompositor();

    if(comp->cursorSurface == surface)
        comp->cursorSurface = nullptr;

    comp->surfacesList.remove(surface);

    // Here you must delete your custon WSurface instance
    delete surface;

    comp->repaint();
}

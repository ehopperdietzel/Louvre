#include "MyClient.h"
#include <MySurface.h>
#include <MyCompositor.h>
#include <string.h>
#include <WOutput.h>

MyClient::MyClient(wl_client *client, WCompositor *compositor) : WClient::WClient(client,compositor){}

WSurface *MyClient::newSurfaceRequest(UInt32 id, wl_resource *resource)
{
    MyCompositor *comp = (MyCompositor*)getCompositor();

    // Get a free texture slot
    MySurface *surface = new MySurface(id,resource,this,1);
    comp->surfacesList.push_back(surface);

    /* Initially all surfaces have undefined type (surface->getType()).
     * You should wait for the WSurface::typeChangeRequest before drawing it.*/

    return surface;
}

void MyClient::surfaceDestroyRequest(WSurface *surf)
{
    MyCompositor *comp = (MyCompositor*)getCompositor();

    MySurface *surface = (MySurface*)surf;

    if(comp->cursorSurface == surface)
        comp->cursorSurface = nullptr;

    if(comp->_keyboardFocusSurface == surface)
        comp->_keyboardFocusSurface = nullptr;

    if(comp->_pointerFocusSurface == surface)
        comp->_pointerFocusSurface = nullptr;

    if(comp->movingSurface == surface)
        comp->movingSurface = nullptr;

    printf("Remove surface:%lu\n",comp->surfacesList.size());
    comp->surfacesList.remove(surface);
    printf("Remove surface:%lu\n",comp->surfacesList.size());

    // Here you must delete your custon WSurface instance
    delete surface;

    comp->repaintAllOutputs();
}

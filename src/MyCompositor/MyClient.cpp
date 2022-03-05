#include "MyClient.h"
#include <MySurface.h>
#include <MyCompositor.h>
#include <string.h>

MyClient::MyClient(wl_client *client, WCompositor *compositor) : WClient::WClient(client,compositor){}

WSurface *MyClient::newSurfaceRequest(UInt32 id, wl_resource *resource)
{
    MyCompositor *comp = (MyCompositor*)getCompositor();
    memset(comp->freeTextureSlots, 0, sizeof(bool)*comp->maxTextureUnits);

    for(list<WClient*>::iterator clientIt = comp->clients.begin(); clientIt != comp->clients.end(); ++clientIt)
    {
        WClient *c = (*clientIt);
        for(list<WSurface*>::iterator surfaceIt = c->surfaces.begin(); surfaceIt != c->surfaces.end(); ++surfaceIt)
        {
            comp->freeTextureSlots[(*surfaceIt)->getTexture()->textureUnit()] = true;
        }
    }

    GLuint freeSlotNumber = 1;

    for(GLuint i = 0; i < comp->maxTextureUnits; i++)
    {
        if(comp->freeTextureSlots[i] == false)
        {
            freeSlotNumber = i;
            break;
        }
    }


    // Get a free texture slot
    MySurface *surface = new MySurface(id,resource,this,freeSlotNumber);
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

    comp->surfacesList.remove((MySurface*)surface);

    // Here you must delete your custon WSurface instance
    delete surface;

    comp->repaint();
}

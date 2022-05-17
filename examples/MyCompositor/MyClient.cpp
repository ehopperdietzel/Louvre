#include "MyClient.h"
#include <MySurface.h>
#include <MyCompositor.h>
#include <string.h>
#include <WOutput.h>
#include "MySeat.h"
#include <WCursor.h>

MyClient::MyClient(wl_client *client, WCompositor *compositor) : WClient::WClient(client,compositor)
{
    comp = (MyCompositor*)compositor;
}

MyClient::~MyClient()
{

}

WSurface *MyClient::newSurfaceRequest(wl_resource *surfaceResource)
{
    // Create your own surface instance
    MySurface *surface = new MySurface(surfaceResource,this,1);

    // Store it for later use
    comp->surfacesList.push_back(surface);

    /* Initially all surfaces have undefined type (surface->getType()).
     * You should wait for the WSurface::typeChangeRequest before drawing it.*/

    return surface;
}

void MyClient::surfaceDestroyRequest(WSurface *surface)
{

    MySeat *seat = (MySeat*)compositor()->seat();

    if(seat->cursorSurface == surface)
        seat->cursorSurface = nullptr;

    if(seat->movingSurface == surface)
        seat->movingSurface = nullptr;

    if(surface->type() == SurfaceType::Cursor)
        comp->cursor->setTexture(comp->defaultCursorTexture,WPointF());

    comp->surfacesList.remove((MySurface*)surface);

    /* Afer this method the compositor will call delete "YourSurface",
     * so make sure to implement all the cleaning up logic in your
     * surface destructor. */

    comp->repaintAllOutputs();
}

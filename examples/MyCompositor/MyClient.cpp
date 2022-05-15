#include "MyClient.h"
#include <MySurface.h>
#include <MyCompositor.h>
#include <string.h>
#include <WOutput.h>
#include "MySeat.h"
#include <WCursor.h>

MyClient::MyClient(wl_client *client, WCompositor *compositor) : WClient::WClient(client,compositor){}

MyClient::~MyClient()
{

}

WSurface *MyClient::newSurfaceRequest(wl_resource *surfaceResource)
{
    MyCompositor *comp = (MyCompositor*)compositor();

    // Create your own surface instance
    MySurface *surface = new MySurface(surfaceResource,this,1);

    // Store it for later use
    comp->surfacesList.push_back(surface);

    /* Initially all surfaces have undefined type (surface->getType()).
     * You should wait for the WSurface::typeChangeRequest before drawing it.*/

    return surface;
}

void MyClient::surfaceDestroyRequest(WSurface *surf)
{
    MyCompositor *comp = (MyCompositor*)compositor();

    MySeat *seat = (MySeat*)compositor()->seat();
    MySurface *surface = (MySurface*)surf;

    if(seat->cursorSurface == surface)
        seat->cursorSurface = nullptr;


    if(seat->keyboardFocusSurface == surface)
        seat->keyboardFocusSurface = nullptr;

    if(seat->pointerFocusSurface == surface)
    {
        seat->pointerFocusSurface = nullptr;
        comp->cursor->setTexture(comp->defaultCursorTexture,WPointF(0,0));
    }

    if(seat->movingSurface == surface)
        seat->movingSurface = nullptr;

    comp->surfacesList.remove(surface);

    /* Afer this method the compositor will call delete "YourSurface",
     * so make sure to implement all the cleaning up logic in your
     * surface destructor. */

    comp->repaintAllOutputs();
}

#include "MyClient.h"
#include <MySurface.h>
#include <MyCompositor.h>
#include <string.h>
#include <WOutput.h>

MyClient::MyClient(wl_client *client, WCompositor *compositor) : WClient::WClient(client,compositor){}

MyClient::~MyClient()
{

}

WSurface *MyClient::newSurfaceRequest(wl_resource *surfaceResource)
{
    MyCompositor *compositor = (MyCompositor*)getCompositor();

    // Create your own surface instance
    MySurface *surface = new MySurface(surfaceResource,this,1);

    // Store it for later use
    compositor->surfacesList.push_back(surface);

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

    comp->surfacesList.remove(surface);

    /* Afer this method the compositor will call delete "YourSurface",
     * so make sure to implement all the cleaning up logic in your
     * surface destructor. */

    comp->repaintAllOutputs();
}

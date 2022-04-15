#include "MyOutputManager.h"
#include <WCompositor.h>

using namespace Wpp;

MyOutputManager::MyOutputManager(Wpp::WCompositor *compositor):WOutputManager::WOutputManager(compositor){}

void MyOutputManager::outputPluggedEvent(Wpp::WOutput *output)
{
    /* Here you can handle new displays connections.
     * Once added to the compositor with compositor->addOutput(output),
     * a new shared OpenGL context will be created (check WCompositor::initializeGL(WOutput *))
     * as well as a new thread for its own render loop.
     * Each output has an unique id (WOutput::getId) which you can use to ...
     */

    getCompositor()->addOutput(output);

}

void MyOutputManager::outputUnpluggedEvent(Wpp::WOutput *output)
{
    /* Handle a display disconnection.
     * Here you could do some cleaning if needed
     * and then remove it from the compositor.
     * After this method, the output is removed from the OutputManager list and destroyed
     * including its OpenGL context.*/

    getCompositor()->removeOutput(output);
}


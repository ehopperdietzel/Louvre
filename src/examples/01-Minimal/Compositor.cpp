#include <Compositor.h>
#include <LSeat.h>
#include <LKeyboard.h>
#include <LOutputManager.h>
#include <LOutput.h>

#include <Output.h>
#include <Surface.h>

Compositor::Compositor(const char *backendPath):LCompositor(backendPath){}

void Compositor::initialize()
{
    // Override the default keymap
    seat()->keyboard()->setKeymap(NULL,NULL,"latam");

    // Add avaliable outputs to the compositor

    Int32 xOffset = 0; // The x position of each output
    for(LOutput *output : *outputManager()->outputs())
    {
        // If output resolution is > 1360 we consider it highDPI
        //if(output->rect(false).w() > 1360)
        output->setOutputScale(2);

        // Set the output position
        output->setPos(LPoint(xOffset,0));

        // Add the output to the compositor (initializes its render context)
        addOutput(output);

        // Increase the x offset for the next output
        xOffset += output->rect(false).w();
    }

}

LOutput *Compositor::createOutputRequest()
{
    return new Output();
}

LSurface *Compositor::createSurfaceRequest(wl_resource *surface, LClient *client)
{
    Surface *newSurface = new Surface(surface, client);
    newSurface->changedOrder[0] = true;
    newSurface->changedOrder[1] = true;

    return newSurface;
}

void Compositor::destroySurfaceRequest(LSurface *s)
{

    if(s->roleType() == LSurface::Cursor || s->minimized())
        return;

    for(Output *o : (list<Output*>&)outputs())
    {
        o->fullRefresh = true;
        o->repaint();
    }
}



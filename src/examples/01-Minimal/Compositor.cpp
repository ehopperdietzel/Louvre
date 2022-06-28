#include <Compositor.h>
#include <LSeat.h>
#include <LKeyboard.h>
#include <LOutputManager.h>
#include <LOutput.h>

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
        if(output->rect(false).w() > 1360)
            output->setOutputScale(2);

        // Set the output position
        output->setPos(LPoint(xOffset,0));

        // Add the output to the compositor (initializes its render context)
        addOutput(output);

        // Increase the x offset for the next output
        xOffset += output->rect(false).w();
    }

}



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

    // Use the output manager to get connected displays
    LOutputManager *outputManager = new LOutputManager(this);

    // Use the first avaliable display
    LOutput *output = outputManager->getOutputsList()->front();

    // Set the output scale (2 for HiDPI displays)
    output->setOutputScale(2);

    // Add the output to the compositor (this inits the output thread, OpenGL context and render loop)
    addOutput(output);
}



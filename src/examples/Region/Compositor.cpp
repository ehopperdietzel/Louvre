#include <Compositor.h>
#include <Output.h>

Compositor::Compositor(const char *backendPath):LCompositor(backendPath){}


LOutput *Compositor::createOutputRequest()
{
    return new Output();
}





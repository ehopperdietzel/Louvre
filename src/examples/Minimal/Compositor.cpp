#include "Compositor.h"
#include "Output.h"

Compositor::Compositor(const char *backend) : LCompositor(backend)
{

}


LOutput *Compositor::createOutputRequest()
{
    return new Output();
}

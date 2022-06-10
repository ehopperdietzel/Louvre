#include <LOutputManager.h>
#include <LOutput.h>
#include <LCompositor.h>

using namespace Louvre;

LOutputManager::LOutputManager(LCompositor *compositor)
{
    p_outputs = compositor->p_backend->getAvaliableOutputs(compositor);
    p_compositor = compositor;
}

LCompositor *LOutputManager::getCompositor() const
{
    return p_compositor;
}

const list<LOutput *> *LOutputManager::getOutputsList()
{
    return &p_outputs;
}


#include <WOutputManager.h>
#include <WOutput.h>
#include <WBackend.h>
#include <WCompositor.h>

using namespace Wpp;

WOutputManager::WOutputManager(WCompositor *compositor)
{
    p_outputs = WBackend::getAvaliableOutputs(compositor);
    p_compositor = compositor;
}

WCompositor *WOutputManager::getCompositor() const
{
    return p_compositor;
}

const list<WOutput *> *WOutputManager::getOutputsList()
{
    return &p_outputs;
}


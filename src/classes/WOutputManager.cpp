#include <WOutputManager.h>
#include <WOutput.h>
#include <WBackend.h>

using namespace WaylandPlus;

WOutputManager::WOutputManager(WCompositor *compositor)
{
    _outputs = WBackend::getAvaliableOutputs();
    _compositor = compositor;
}

WCompositor *WOutputManager::getCompositor()
{
    return _compositor;
}

const list<WOutput *> *WOutputManager::getOutputsList()
{
    return &_outputs;
}


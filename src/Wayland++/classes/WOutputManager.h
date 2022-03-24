#ifndef WOUTPUTMANAGER_H
#define WOUTPUTMANAGER_H

#include <WNamespaces.h>
#include <list>

using namespace std;

class WaylandPlus::WOutputManager
{
public:
    WOutputManager(WCompositor *compositor);
    WCompositor *getCompositor();
    const list<WOutput*>*getOutputsList();

protected:
    virtual void outputPluggedEvent(WOutput *output) = 0;
    virtual void outputUnpluggedEvent(WOutput *output) = 0;

private:
    WCompositor *_compositor = nullptr;
    list<WOutput*>_outputs;
};

#endif // WOUTPUTMANAGER_H

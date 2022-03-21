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
private:
    WCompositor *_compositor = nullptr;
    list<WOutput*>_outputs;
};

#endif // WOUTPUTMANAGER_H

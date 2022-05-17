#ifndef WOUTPUTMANAGER_H
#define WOUTPUTMANAGER_H

#include <WNamespaces.h>
#include <list>

using namespace std;

class Wpp::WOutputManager
{
public:
    WOutputManager(WCompositor *compositor);
    WCompositor *getCompositor() const;
    const list<WOutput*>*getOutputsList();

protected:
    virtual void outputPluggedEvent(WOutput *output) = 0;
    virtual void outputUnpluggedEvent(WOutput *output) = 0;

private:
    WCompositor *p_compositor = nullptr;
    list<WOutput*>p_outputs;
};

#endif // WOUTPUTMANAGER_H

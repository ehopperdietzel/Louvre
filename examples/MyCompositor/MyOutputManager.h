#ifndef MYOUTPUTMANAGER_H
#define MYOUTPUTMANAGER_H

#include <WNamespaces.h>
#include <WOutputManager.h>

class MyOutputManager : public WaylandPlus::WOutputManager
{
public:
    MyOutputManager(WaylandPlus::WCompositor*compositor);
private:
    void outputPluggedEvent(WaylandPlus::WOutput *output) override;
    void outputUnpluggedEvent(WaylandPlus::WOutput *output) override;
};

#endif // MYOUTPUTMANAGER_H

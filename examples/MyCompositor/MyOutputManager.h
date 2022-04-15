#ifndef MYOUTPUTMANAGER_H
#define MYOUTPUTMANAGER_H

#include <WNamespaces.h>
#include <WOutputManager.h>

class MyOutputManager : public Wpp::WOutputManager
{
public:
    MyOutputManager(Wpp::WCompositor*compositor);
private:
    void outputPluggedEvent(Wpp::WOutput *output) override;
    void outputUnpluggedEvent(Wpp::WOutput *output) override;
};

#endif // MYOUTPUTMANAGER_H

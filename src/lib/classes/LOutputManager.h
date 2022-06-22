#ifndef LOUTPUTMANAGER_H
#define LOUTPUTMANAGER_H

#include <LNamespaces.h>
#include <list>

using namespace std;

class Louvre::LOutputManager
{
public:
    LOutputManager(LCompositor *compositor);
    LCompositor *getCompositor() const;
    const list<LOutput*>*getOutputsList();

private:
    LCompositor *m_compositor = nullptr;
    list<LOutput*>m_outputs;
};

#endif // LOUTPUTMANAGER_H

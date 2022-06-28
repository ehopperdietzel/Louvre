#ifndef LOUTPUTMANAGER_H
#define LOUTPUTMANAGER_H

#include <LNamespaces.h>
#include <list>

using namespace std;

class Louvre::LOutputManager
{
public:
    LOutputManager(LCompositor *compositor);
    virtual ~LOutputManager();

    virtual void connectedOutputRequest(LOutput *connectedOutput);
    virtual void disonnectedOutputRequest(LOutput *disconnectedOutput);

    LCompositor *compositor() const;
    const list<LOutput*>*outputs() const;
    class LOutputManagerPrivate;
    LOutputManagerPrivate *imp() const;
private:
    LOutputManagerPrivate *m_imp = nullptr;
};

#endif // LOUTPUTMANAGER_H

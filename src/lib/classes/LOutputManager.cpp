#include <LOutputManagerPrivate.h>
#include <LCompositorPrivate.h>
#include <LOutput.h>

using namespace Louvre;

LOutputManager::LOutputManager(LCompositor *compositor)
{
    m_imp = new LOutputManagerPrivate();
    m_imp->m_outputs = compositor->imp()->m_backend->getAvaliableOutputs(compositor);
    m_imp->m_compositor = compositor;
}

LOutputManager::~LOutputManager()
{
    delete m_imp;
}

void LOutputManager::pluggedOutputRequest(LOutput *connectedOutput)
{
    if(!compositor()->outputs().empty())
    {
        LRect prevRect = compositor()->outputs().back()->rect();
        connectedOutput->setPos(LPoint(prevRect.x()+prevRect.w(),0));
    }
    compositor()->addOutput(connectedOutput);
}

void LOutputManager::unpluggedOutputRequest(LOutput *disconnectedOutput)
{
    compositor()->removeOutput(disconnectedOutput);
}

LCompositor *LOutputManager::compositor() const
{
    return m_imp->m_compositor;
}

const list<LOutput *> *LOutputManager::outputs() const
{
    return m_imp->m_outputs;
}


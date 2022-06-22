#include <LOutputManager.h>
#include <LOutput.h>
#include <LCompositorPrivate.h>

using namespace Louvre;

LOutputManager::LOutputManager(LCompositor *compositor)
{
    m_outputs = compositor->imp()->m_backend->getAvaliableOutputs(compositor);
    m_compositor = compositor;
}

LCompositor *LOutputManager::getCompositor() const
{
    return m_compositor;
}

const list<LOutput *> *LOutputManager::getOutputsList()
{
    return &m_outputs;
}


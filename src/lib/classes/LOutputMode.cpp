#include <LOutputModePrivate.h>

using namespace Louvre;

LOutputMode::LOutputMode(UInt32 width, UInt32 height, UInt32 refreshRate)
{
    m_imp = new LOutputModePrivate();
    m_imp->m_size.setW(width);
    m_imp->m_size.setH(height);
    m_imp->m_refreshRate = refreshRate;
}

LOutputMode::~LOutputMode()
{
    delete m_imp;
}

const LSize &LOutputMode::size() const
{
    return m_imp->m_size;
}

UInt32 LOutputMode::refreshRate() const
{
    return m_imp->m_refreshRate;
}

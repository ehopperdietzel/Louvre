#ifndef LOUTPUTMODEPRIVATE_H
#define LOUTPUTMODEPRIVATE_H

#include <LOutputMode.h>
#include <LSize.h>

class Louvre::LOutputMode::LOutputModePrivate
{
public:
    LOutputModePrivate() = default;
    ~LOutputModePrivate() = default;

    LSize m_size;
    UInt32 m_refreshRate = 0;
    void *m_backendData = nullptr;
};


#endif // LOUTPUTMODEPRIVATE_H

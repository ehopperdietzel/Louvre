#ifndef LOUTPUTMANAGERPRIVATE_H
#define LOUTPUTMANAGERPRIVATE_H

#include <LOutputManager.h>

class Louvre::LOutputManager::LOutputManagerPrivate
{
public:
    LOutputManagerPrivate() = default;
    ~LOutputManagerPrivate() = default;

    LCompositor *m_compositor = nullptr;
    list<LOutput*> *m_outputs = nullptr;
};

#endif // LOUTPUTMANAGERPRIVATE_H

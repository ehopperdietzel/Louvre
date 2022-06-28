#ifndef LOUTPUTMODE_H
#define LOUTPUTMODE_H

#include <LNamespaces.h>

class Louvre::LOutputMode
{
public:
    LOutputMode(UInt32 width, UInt32 height, UInt32 refreshRate);
    ~LOutputMode();

    const LSize &size() const;
    UInt32 refreshRate() const;

    class LOutputModePrivate;
    LOutputModePrivate *imp() const;
private:
    LOutputModePrivate *m_imp = nullptr;
};

#endif // LOUTPUTMODE_H

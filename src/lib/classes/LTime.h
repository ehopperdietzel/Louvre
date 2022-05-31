#ifndef LTIME_H
#define LTIME_H

#include <LNamespaces.h>

class Louvre::LTime
{
public:
    static UInt32 ms();
    static timespec us();
};

#endif // LTIME_H

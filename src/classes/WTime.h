#ifndef WTIME_H
#define WTIME_H

#include <WNamespaces.h>

class Wpp::WTime
{
public:
    static UInt32 ms();
    static timespec us();
};

#endif // WTIME_H

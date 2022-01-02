#ifndef WTYPES_H
#define WTYPES_H

#include <libinput.h>

typedef uint32_t    UInt32;
typedef int32_t     Int32;

struct Rect
{
    Int32 x = 0;
    Int32 y = 0;
    Int32 width = 0;
    Int32 height = 0;
};

struct WRegionRect
{
    Int32 x = 0;
    Int32 y = 0;
    Int32 width = 0;
    Int32 height = 0;
    bool add = true;
};

#endif // WTYPES_H

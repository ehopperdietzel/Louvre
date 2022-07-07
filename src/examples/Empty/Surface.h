#ifndef SURFACE_H
#define SURFACE_H

#include <LSurface.h>

using namespace Louvre;

class Surface : public LSurface
{
public:
    Surface(wl_resource *surface, LClient *client, GLuint textureUnit = 1);

    /* void roleChanged() override; */
    /* void parentChanged() override; */
    /* void bufferScaleChanged() override; */
    /* void bufferSizeChanged() override; */
    /* void opaqueRegionChanged() override; */
    /* void inputRegionChanged() override; */
};

#endif // SURFACE_H

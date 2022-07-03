#ifndef COMPOSITOR_H
#define COMPOSITOR_H

#include <LCompositor.h>

using namespace Louvre;

class Compositor : public LCompositor
{
public:
    Compositor(const char *backendPath);

    void initialize() override;

    LOutput *createOutputRequest() override;
    LSurface *createSurfaceRequest(wl_resource *surface, LClient *client) override;

    void destroySurfaceRequest(LSurface *surface) override;
};

#endif // COMPOSITOR_H

#ifndef MYSURFACE_H
#define MYSURFACE_H

#include <WSurface.h>

using namespace WaylandPlus;

class MySurface : public WSurface
{
public:
    MySurface(UInt32 id, wl_resource *resource, WClient *client);
    ~MySurface();

    // Requests
    void moveStartRequest() override;
    void maxSizeChangeRequest(Int32 width, Int32 height) override;
    void minSizeChangeRequest(Int32 width, Int32 height) override;
    void resizeStartRequest(ResizeEdge edge) override;
    void geometryChangeRequest(Int32 x, Int32 y, Int32 width, Int32 height) override;
    void typeChangeRequest() override;
    void positionerChangeRequest() override;
    void parentChangeRequest() override;
};

#endif // MYSURFACE_H

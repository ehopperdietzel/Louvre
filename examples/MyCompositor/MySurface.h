#ifndef MYSURFACE_H
#define MYSURFACE_H

#include <WSurface.h>

using namespace Wpp;

class MyCompositor;

class MySurface : public WSurface
{
public:
    MySurface(wl_resource *surfaceResource, WClient *client, GLuint textureUnit);
    ~MySurface();

    // Requests
    void moveStartRequest() override;
    void maxSizeChangeRequest() override;
    void minSizeChangeRequest() override;
    void resizeStartRequest(ResizeEdge edge) override;
    void geometryChangeRequest() override;
    void typeChangeRequest() override;
    void positionerChangeRequest() override;
    void parentChangeRequest() override;
    void bufferScaleChangeRequest() override;
    void bufferSizeChangeRequest() override;
    void grabSeatRequest() override;

    MyCompositor *comp = nullptr;

    Int32 mapXtoLocal(int xGlobal);
    Int32 mapYtoLocal(int yGlobal);

    bool containsPoint(Int32 x, Int32 y, bool withoutDecoration = false);
    WRect getRectWithoutDecoration();
    void setXWithoutDecoration(Int32 x);
    void setYWithoutDecoration(Int32 y);

    // Surface position
    WPoint pos;
};

#endif // MYSURFACE_H

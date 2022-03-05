#ifndef MYSURFACE_H
#define MYSURFACE_H

#include <WSurface.h>

using namespace WaylandPlus;

class MySurface : public WSurface
{
public:
    MySurface(UInt32 id, wl_resource *resource, WClient *client, GLuint textureUnit);
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

    Int32 mapXtoLocal(int xGlobal);
    Int32 mapYtoLocal(int yGlobal);

    bool containsPoint(Int32 x, Int32 y, bool withoutDecoration = false);
    Rect getRectWithoutDecoration();
    void setPos(int x, int y);
    void setX(int x);
    void setY(int y);
    void setXWithoutDecoration(Int32 x);
    void setYWithoutDecoration(Int32 y);
    int getX();
    int getY();

    Int32 _posX = 0;
    Int32 _posY = 0;
};

#endif // MYSURFACE_H

#include "WSurface.h"

WSurface::WSurface(wl_resource *res)
{
    resource = res;
}

void WSurface::setPos(int x, int y)
{
    _posX = x;
    _posY = y;
}

void WSurface::setX(int x)
{
    _posX = x;
}

void WSurface::setY(int y)
{
    _posY = y;
}

int WSurface::getX()
{
    return _posX;
}

int WSurface::getY()
{
    return _posY;
}

int WSurface::getWidth()
{
    return texture->width();
}

int WSurface::getHeight()
{
    return texture->height();
}

int WSurface::mapXtoLocal(int xGlobal)
{
    return xGlobal - getX();
}

int WSurface::mapYtoLocal(int yGlobal)
{
    return yGlobal - getY();
}

bool WSurface::containsPoint(int x, int y)
{
    if(_posX > x)
        return false;
    if(_posX + getWidth() < x)
        return false;
    if(_posY > y)
        return false;
    if(_posY + getHeight() < y)
        return false;

    return true;
}

int32_t WSurface::getBufferScale()
{
    return _bufferScale;
}

void WSurface::setBufferScale(int32_t scale)
{
    _bufferScale = scale;
}

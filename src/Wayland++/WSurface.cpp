#include "WSurface.h"
#include <WClient.h>
WSurface::WSurface(wl_resource *res)
{
    resource = res;
}

void WSurface::sendPointerButtonEvent(UInt32 buttonCode, UInt32 buttonState, UInt32 milliseconds)
{
    if(client->pointer)
    {
        wl_pointer_send_button(client->pointer,0,milliseconds,buttonCode,buttonState);
        wl_pointer_send_frame(client->pointer);
    }
}

void WSurface::sendPointerMotionEvent(double x, double y, UInt32 milliseconds)
{
    if(client->pointer)
    {
        wl_pointer_send_motion(
                    client->pointer,
                    milliseconds,
                    wl_fixed_from_double(x),
                    wl_fixed_from_double(y));

        wl_pointer_send_frame(client->pointer);
    }
}

void WSurface::sendPointerEnterEvent(double x, double y)
{
    if(client->pointer)
    {
        wl_pointer_send_enter(
                    client->pointer,
                    0,
                    resource,
                    wl_fixed_from_double(x),
                    wl_fixed_from_double(y));
    }
}

void WSurface::sendPointerLeaveEvent()
{
    if(client->pointer)
        wl_pointer_send_leave(client->pointer,0,resource);
}

void WSurface::sendKeyEvent(UInt32 keyCode, UInt32 keyState, UInt32 milliseconds)
{
    if(client->keyboard)
        wl_keyboard_send_key(client->keyboard,0,milliseconds,keyCode,keyState);
}

void WSurface::sendKeyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    if(client->keyboard)
        wl_keyboard_send_modifiers(client->keyboard,0,depressed,latched,locked,group);
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

Int32 WSurface::getBufferScale()
{
    return _bufferScale;
}

void WSurface::setBufferScale(Int32 scale)
{
    _bufferScale = scale;
}

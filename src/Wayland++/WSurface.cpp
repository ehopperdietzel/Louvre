#include "WSurface.h"
#include <WClient.h>
#include <WCompositor.h>

#include <time.h>
#include <stdlib.h>


wl_array nullKeys;

void createNullKeys()
{
    wl_array_init(&nullKeys);
}

WSurface::WSurface(UInt32 id, wl_resource *res, WClient *client)
{
    srand(time(NULL));
    _resource = res;
    _client = client;
    _id = id;
    setX(rand() % 50);
    setY(rand() % 50);
}

void WSurface::sendPointerButtonEvent(UInt32 buttonCode, UInt32 buttonState, UInt32 milliseconds)
{
    if(_client->getPointer())
    {
        wl_pointer_send_button(_client->getPointer(),0,milliseconds,buttonCode,buttonState);
        wl_pointer_send_frame(_client->getPointer());
    }
}

void WSurface::sendPointerMotionEvent(double x, double y, UInt32 milliseconds)
{
    if(_client->getPointer())
    {
        wl_pointer_send_motion(
                    _client->getPointer(),
                    milliseconds,
                    wl_fixed_from_double(x),
                    wl_fixed_from_double(y));

        wl_pointer_send_frame(_client->getPointer());
    }
}

void WSurface::sendPointerEnterEvent(double x, double y)
{
    if(getCompositor()->_pointerFocusSurface == this)
        return;

    // Send previus surface a leave event
    if(getCompositor()->getPointerFocusSurface())
        getCompositor()->getPointerFocusSurface()->sendPointerLeaveEvent();

    if(_client->getPointer())
    {
        wl_pointer_send_enter(
                    _client->getPointer(),
                    0,
                    _resource,
                    wl_fixed_from_double(x),
                    wl_fixed_from_double(y));

        getCompositor()->_pointerFocusSurface = this;
    }
}

void WSurface::sendPointerLeaveEvent()
{
    if(getCompositor()->_pointerFocusSurface != this)
        return;

    if(_client->getPointer())
        wl_pointer_send_leave(_client->getPointer(),0,_resource);

    getCompositor()->_pointerFocusSurface = nullptr;
}

void WSurface::sendKeyEvent(UInt32 keyCode, UInt32 keyState, UInt32 milliseconds)
{
    if(_client->getKeyboard())
        wl_keyboard_send_key(_client->getKeyboard(),0,milliseconds,keyCode,keyState);
}

void WSurface::sendKeyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    if(_client->getKeyboard())
        wl_keyboard_send_modifiers(_client->getKeyboard(),0,depressed,latched,locked,group);
}

void WSurface::sendKeyboardEnterEvent()
{
    if(getCompositor()->_keyboardFocusSurface == this)
        return;

    if(_client->getKeyboard())
        wl_keyboard_send_enter(_client->getKeyboard(),0,_resource,&nullKeys);

    getCompositor()->_keyboardFocusSurface = this;
}

void WSurface::sendKeyboardLeaveEvent()
{
    if(getCompositor()->_keyboardFocusSurface != this)
        return;

    if(_client->getKeyboard())
        wl_keyboard_send_leave(_client->getKeyboard(),0,_resource);

    getCompositor()->_keyboardFocusSurface = nullptr;
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

wl_resource *WSurface::getResource()
{
    return _resource;
}

WClient *WSurface::getClient()
{
    return _client;
}

WCompositor *WSurface::getCompositor()
{
    return _client->getCompositor();
}

UInt32 WSurface::getId()
{
    return _id;
}



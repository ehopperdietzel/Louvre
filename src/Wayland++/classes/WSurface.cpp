#include "WSurface.h"
#include <WClient.h>
#include <WCompositor.h>

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include <xdg-shell.h>

using namespace WaylandPlus;

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

WSurface::~WSurface()
{
    delete []_appId;
    delete []_title;
}

void WSurface::sendPointerButtonEvent(UInt32 buttonCode, UInt32 buttonState, UInt32 milliseconds)
{
    if(_client->getPointer())
    {
        wl_pointer_send_button(_client->getPointer(),pointerSerial,milliseconds,buttonCode,buttonState);
        pointerSerial++;
        if(_client->_wl_pointer_version >= 5)
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

        if(_client->_wl_pointer_version >= 5)
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
                    pointerSerial,
                    _resource,
                    wl_fixed_from_double(x),
                    wl_fixed_from_double(y));
        pointerSerial++;
        if(_client->_wl_pointer_version >= 5)
            wl_pointer_send_frame(_client->getPointer());
        getCompositor()->_pointerFocusSurface = this;
    }
}

void WSurface::sendPointerLeaveEvent()
{
    if(getCompositor()->_pointerFocusSurface != this)
        return;

    if(_client->getPointer())
    {
        wl_pointer_send_leave(_client->getPointer(),pointerSerial,_resource);
        pointerSerial++;
        if(_client->_wl_pointer_version >= 5)
            wl_pointer_send_frame(_client->getPointer());
    }

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

void WSurface::sendConfigureEvent(Int32 width, Int32 height, SurfaceStateFlags states)
{
    if(xdg_toplevel != nullptr) //xdgShellVersion >= 2
    {
        wl_array dummy;
        wl_array_init(&dummy);
        UInt32 index = 0;

        if((states & (SurfaceStateFlags)SurfaceState::Activated))
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_ACTIVATED;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::Fullscreen)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_FULLSCREEN;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::Maximized)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_MAXIMIZED;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::Resizing)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_RESIZING;
            index++;
        }

        if(states & (SurfaceStateFlags)SurfaceState::TiledBottom)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_BOTTOM;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::TiledLeft)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_LEFT;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::TiledRight)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_RIGHT;
            index++;
        }
        if(states & (SurfaceStateFlags)SurfaceState::TiledTop)
        {
            wl_array_add(&dummy, sizeof(xdg_toplevel_state));
            xdg_toplevel_state *s = (xdg_toplevel_state*)dummy.data;
            s[index] = XDG_TOPLEVEL_STATE_TILED_TOP;
            index++;
        }

        xdg_toplevel_send_configure(xdg_toplevel,width,height,&dummy);
        wl_array_release(&dummy);
        xdg_surface_send_configure(xdg_shell,configureSerial);
        configureSerial++;
    }
}

void WSurface::setAppId(const char *appId)
{
    delete _appId;
    _appId = new char[strlen(appId)+1];
    strcpy(_appId,appId);
}

void WSurface::setTitle(const char *title)
{
    delete _title;
    _title = new char[strlen(title)+1];
    strcpy(_title,title);
}

const char *WSurface::getAppId()
{
    return _appId;
}

const char *WSurface::getTitle()
{
    return _title;
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

void WSurface::setXWithoutDecoration(Int32 x)
{
    setX(x-_decorationGeometry.x);
}

void WSurface::setYWithoutDecoration(Int32 y)
{
    setY(y-_decorationGeometry.y);
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

Rect WSurface::getRectWithoutDecoration()
{
    Rect rect;
    if(xdg_shell != nullptr)
    {
        rect.x = getX() + _decorationGeometry.x;
        rect.y = getY() + _decorationGeometry.y;
        rect.width = _decorationGeometry.width;
        rect.height = _decorationGeometry.height;
    }
    else
    {
        rect.x = getX();
        rect.y = getY();
        rect.width = getWidth();
        rect.height = getHeight();
    }
    return rect;
}

WPositioner *WSurface::getPositioner()
{
    return _positioner;
}

Int32 WSurface::getMinWidth()
{
    return _minWidth;
}

Int32 WSurface::getMinHeight()
{
    return _minHeight;
}

Int32 WSurface::getMaxWidth()
{
    return _maxWidth;
}

Int32 WSurface::getMaxHeight()
{
    return _maxHeight;
}

void WSurface::setMinWidth(Int32 width)
{
    _minWidth = width;
}

void WSurface::setMinHeight(Int32 height)
{
    _minHeight = height;
}

void WSurface::setMaxWidth(Int32 width)
{
    _maxWidth = width;
}

void WSurface::setMaxHeight(Int32 height)
{
    _maxHeight = height;
}

int WSurface::mapXtoLocal(int xGlobal)
{
    return xGlobal - getX();
}

int WSurface::mapYtoLocal(int yGlobal)
{
    return yGlobal - getY();
}

bool WSurface::containsPoint(Int32 x, Int32 y, bool withoutDecoration)
{
    if(withoutDecoration)
    {
        Rect r = getRectWithoutDecoration();

        if(r.x > x)
            return false;
        if(r.x + r.width < x)
            return false;
        if(r.y> y)
            return false;
        if(r.y + r.height < y)
            return false;

        return true;
    }
    else
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

SurfaceType WSurface::getType()
{
    return _type;
}

WaylandPlus::WSurface *WSurface::getParent()
{

}

const list<WaylandPlus::WSurface *> WSurface::getChildren()
{

}



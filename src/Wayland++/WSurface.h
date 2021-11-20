#ifndef WSURFACE_H
#define WSURFACE_H

#include <wayland-server.h>
#include <WTexture.h>
#include <WTypes.h>

class WClient;

class WSurface
{
public:
    WSurface(wl_resource *res);

    WTexture *texture = new WTexture();

    WClient *client = nullptr;

    wl_resource *resource = nullptr;
    wl_resource *frame_callback = nullptr;
    wl_resource *buffer = nullptr;

    wl_resource *xdg_shell = nullptr;
    wl_resource *xdg_toplevel = nullptr;
    wl_resource *xdg_popup = nullptr;

    // Events
    void sendPointerButtonEvent(UInt32 buttonCode, UInt32 buttonState, UInt32 milliseconds);
    void sendPointerMotionEvent(double x, double y, UInt32 milliseconds);
    void sendPointerEnterEvent(double x, double y);
    void sendPointerLeaveEvent();
    void sendKeyEvent(UInt32 keyCode, UInt32 keyState, UInt32 milliseconds);
    void sendKeyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group);

    void setPos(int x, int y);
    void setX(int x);
    void setY(int y);
    int getX();
    int getY();

    int getWidth();
    int getHeight();

    int mapXtoLocal(int xGlobal);
    int mapYtoLocal(int yGlobal);

    bool containsPoint(int x, int y);

    Int32 getBufferScale();
    void setBufferScale(Int32 scale);

 private:
    int _posX = 0;
    int _posY = 0;
    int32_t _bufferScale = 1;

};

#endif // WSURFACE_H

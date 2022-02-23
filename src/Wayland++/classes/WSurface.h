#ifndef WSURFACE_H
#define WSURFACE_H

#include <wayland-server.h>
#include <WTexture.h>
#include <WNamespaces.h>

void createNullKeys();

class WaylandPlus::WSurface
{
public:
    WSurface(UInt32 id, wl_resource *res, WClient *client);

    WTexture *texture = new WTexture();

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
    void sendKeyboardEnterEvent();
    void sendKeyboardLeaveEvent();

    void setAppId(const char *appId);
    void setTitle(const char *title);
    char *getAppId();
    char *getTitle();

    void setPos(int x, int y);
    void setX(int x);
    void setY(int y);
    int getX();
    int getY();

    int getWidth();
    int getHeight();

    Int32 getMinWidth();
    Int32 getMinHeight();
    Int32 getMaxWidth();
    Int32 getMaxHeight();

    void setMinWidth(Int32 width);
    void setMinHeight(Int32 height);
    void setMaxWidth(Int32 width);
    void setMaxHeight(Int32 height);

    int mapXtoLocal(int xGlobal);
    int mapYtoLocal(int yGlobal);

    bool containsPoint(int x, int y);

    Int32 getBufferScale();
    void setBufferScale(Int32 scale);

    wl_resource *getResource();
    WClient *getClient();
    WCompositor *getCompositor();
    UInt32 getId();

 private:
    WClient *_client = nullptr;
    wl_resource *_resource = nullptr;
    int _posX = 0;
    int _posY = 0;
    Int32 _maxWidth,_maxHeight,_minWidth,_minHeight = -1;
    Int32 _bufferScale = 1;
    UInt32 _id;
    char *_appId = nullptr;
    char *_title = nullptr;



};

#endif // WSURFACE_H

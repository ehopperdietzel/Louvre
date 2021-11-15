#ifndef WSURFACE_H
#define WSURFACE_H

#include <wayland-server.h>
#include <WTexture.h>

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

 private:
    int _posX = 0;
    int _posY = 0;

};

#endif // WSURFACE_H

#ifndef MYCOMPOSITOR_H
#define MYCOMPOSITOR_H

#include <WCompositor.h>
#include <MySurface.h>
#include <WPointF.h>

using namespace Wpp;

class MyCompositor : public WCompositor
{
public:
    MyCompositor();

    // Virtual methods to override
    void initializeGL(WOutput *output) override;
    void paintGL(WOutput *output) override;
    virtual WSeat *configureSeat() override;

    WClient *newClientRequest(wl_client *client) override;
    void clientDisconnectRequest(WClient *client) override;

    // Surfaces list ( orderer from back to front )
    list<MySurface*>surfacesList;

    WTexture *defaultCursorTexture,*backgroundTexture;
    WPoint backgroundPos;

    void drawSurfaceTree(WOutput *output, MySurface *surface);
    void riseSurface(MySurface *surface);

    WCursor *cursor;

};

#endif // MYCOMPOSITOR_H

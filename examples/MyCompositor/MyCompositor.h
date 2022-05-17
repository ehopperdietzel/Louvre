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

    void riseSurface(MySurface *surface);
    MySurface *surfaceAt(const WPoint &point);

    WCursor *cursor = nullptr;

};

#endif // MYCOMPOSITOR_H

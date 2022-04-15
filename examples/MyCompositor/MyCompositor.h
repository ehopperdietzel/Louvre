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

    WClient *newClientRequest(wl_client *client) override;
    void clientDisconnectRequest(WClient *client) override;

    void setCursorRequest(WSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY) override;

    void libinputEvent(libinput_event *ev) override;
    void pointerMoveEvent(float dx, float dy) override;
    void pointerClickEvent(UInt32 button, UInt32 state) override;
    void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) override;
    void keyEvent(UInt32 keyCode, UInt32 keyState) override;


    void setPointerPos(double x, double y);
    WSurface *_pointerFocusSurface = nullptr;
    WSurface *_keyboardFocusSurface = nullptr;
    WSurface *_cursorSurface = nullptr;
    WPointF cursorHotspot;
    WPointF pointer;
    WOutput *pointerOutput = nullptr;

    // Square
    GLfloat square[16] =
    {
         0.0f, 0.0f,  0.f, 1.f,
         0.0f, 1.0f,  0.f, 0.f,
         1.0f, 1.0f,  1.f, 0.f,
         1.0f, 0.0f,  1.f, 1.f
    };

    // Uniform variables
    GLuint
    screenSizeUniform,          // Screen size (width,height)
    texSizeUniform,             // Texture size (width,height)
    dstRectUniform,             // Dest quad position and size (x,y,width,height)
    srcRectUniform,             // Src tex rect (x,y,width,height)
    activeTextureUniform;       // glActiveTexture

    // Active surfaces
    MySurface *focusSurface,*movingSurface,*resizingSurface,*cursorSurface = nullptr;

    // Active resize edge
    ResizeEdge resizeEdge;

    // Resize surface initial rect
    WRect resizeInitSurfaceRect;
    WRect resizeInitSurfaceDecoration;

    // Resize surface initial mouse pos
    WPoint resizeInitMousePos;

    // Surfaces list ( orderer from back to front )
    list<MySurface*>surfacesList;

    WTexture *defaultCursorTexture = new WTexture();
    WTexture *backgroundTexture = new WTexture();
    WPoint backgroundPos;

    WPoint movingSurfaceInitPos, movingSurfaceInitCursorPos;

    bool isLeftMouseButtonPressed = false;

    void drawCursor();

    void drawQuad(WTexture *tex,WRect src, WRect dst);

};

#endif // MYCOMPOSITOR_H

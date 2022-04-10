#ifndef MYCOMPOSITOR_H
#define MYCOMPOSITOR_H

#include <WCompositor.h>
#include <MySurface.h>

using namespace WaylandPlus;

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
    Point cursorHotspot = {0,0};
    PointD pointer;
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
    screenUniform,              // Tamaño de la pantalla (width,height)
    rectUniform,                // Posición y tamaño del cuadrado a dibujar
    borderRadiusUniform,        // Radio de los bordes redondeados
    borderRadiusCornersUniform, // Esquinas activas del border radius (TL,TR,BL,BR)
    shadowCornerUniform,        // Indica la sombra a dibujar -1=Para no dibujar 0 = TL
    shadowConfUniform,          // Indica el xOffset, yOffsety radio de la sombra
    activeTextureUniform;       // glActiveTexture

    // Active surfaces
    MySurface *focusSurface,*movingSurface,*resizingSurface,*cursorSurface = nullptr;

    // Active resize edge
    ResizeEdge resizeEdge;

    // Resize surface initial rect
    RectD resizeInitialSurfaceRect;
    Rect resizeInitialSurfaceDecoration;

    // Resize surface initial mouse pos
    PointD resizeInitialMousePos;

    // Surfaces list ( orderer from back to front )
    list<MySurface*>surfacesList;

    WTexture *defaultCursorTexture = new WTexture();
    WTexture *backgroundTexture = new WTexture();
    Point backgroundPos;

    Int32 movingSurfaceInitialPosX,movingSurfaceInitialPosY,movingSurfaceInitialCursorPosX,movingSurfaceInitialCursorPosY = 0;

    bool isLeftMouseButtonPressed = false;
    void drawCursor();

};

#endif // MYCOMPOSITOR_H
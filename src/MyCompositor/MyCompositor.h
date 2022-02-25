#ifndef MYCOMPOSITOR_H
#define MYCOMPOSITOR_H

#include <WCompositor.h>

using namespace WaylandPlus;

class MyCompositor : public WCompositor
{
public:
    MyCompositor();

    // Virtual methods to override
    void initializeGL() override;
    void paintGL() override;

    void newClient(WClient *client) override;
    void clientDisconnected(WClient *client) override;

    void newSurface(WSurface *surface) override;
    void surfaceDestroyed(WSurface *surface) override;
    void surfaceMoveEvent(WSurface *surface) override;
    void surfaceMaxSizeChanged(WSurface *surface, Int32 width, Int32 height) override;
    void surfaceMinSizeChanged(WSurface *surface, Int32 width, Int32 height) override;
    void surfaceResizeRequest(WSurface *surface, ResizeEdge edge) override;
    void surfaceGeometryChangedRequest(WSurface *surface, Int32 x, Int32 y, Int32 width, Int32 height) override;

    void setCursorRequest(WSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY) override;

    void libinputEvent(libinput_event *ev) override;
    void pointerPosChanged(double x, double y, UInt32 milliseconds) override;
    void pointerClickEvent(int x, int y, UInt32 button, UInt32 state, UInt32 milliseconds) override;
    void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) override;
    void keyEvent(UInt32 keyCode,UInt32 keyState,UInt32 milliseconds) override;

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
    shadowConfUniform;          // Indica el xOffset, yOffsety radio de la sombra

    // Active surfaces
    WSurface *focusSurface,*movingSurface,*resizingSurface,*cursorSurface = nullptr;

    // Active resize edge
    ResizeEdge resizeEdge;

    // Resize surface initial rect
    RectD resizeInitialSurfaceRect;

    // Resize surface initial mouse pos
    PointD resizeInitialMousePos;

    // Surfaces list ( orderer from back to front )
    list<WSurface*>surfacesList;

    WTexture *defaultCursorTexture = nullptr;

    Int32 movingSurfaceInitialPosX,movingSurfaceInitialPosY,movingSurfaceInitialCursorPosX,movingSurfaceInitialCursorPosY = 0;

    bool isLeftMouseButtonPressed = false;
    void drawCursor();
};

#endif // MYCOMPOSITOR_H

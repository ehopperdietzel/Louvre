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
    void initializeGL() override;
    void paintGL() override;

    WClient *newClientRequest(wl_client *client) override;
    void clientDisconnectRequest(WClient *client) override;

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
    shadowConfUniform,          // Indica el xOffset, yOffsety radio de la sombra
    activeTextureUniform;       // glActiveTexture

    // Active surfaces
    MySurface *focusSurface,*movingSurface,*resizingSurface,*cursorSurface = nullptr;

    // Active resize edge
    ResizeEdge resizeEdge;

    // Resize surface initial rect
    RectD resizeInitialSurfaceRect;

    // Resize surface initial mouse pos
    PointD resizeInitialMousePos;

    // Surfaces list ( orderer from back to front )
    list<MySurface*>surfacesList;

    WTexture *defaultCursorTexture = nullptr;

    bool *freeTextureSlots = nullptr;

    Int32 movingSurfaceInitialPosX,movingSurfaceInitialPosY,movingSurfaceInitialCursorPosX,movingSurfaceInitialCursorPosY = 0;

    bool isLeftMouseButtonPressed = false;
    void drawCursor();

    GLuint maxTextureUnits;
};

#endif // MYCOMPOSITOR_H

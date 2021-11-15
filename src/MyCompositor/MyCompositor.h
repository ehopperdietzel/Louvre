#ifndef MYCOMPOSITOR_H
#define MYCOMPOSITOR_H

#include <WCompositor.h>

class MyCompositor : public WCompositor
{
public:
    MyCompositor();

    // Virtual methods to override
    void initializeGL() override;
    void paintGL() override;
    void libinputEvent(libinput_event *ev) override;
    void pointerPosChanged(double x, double y) override;
    void pointerClickEvent(int x, int y, uint32_t button, libinput_button_state state) override;

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

    // Focus surface
    WSurface *focusSurface = nullptr;

    void drawCursor();
};

#endif // MYCOMPOSITOR_H

#include "MySeat.h"
#include "MyCompositor.h"
#include <WCursor.h>
#include <WWayland.h>
#include <unistd.h>
#include <WPoint.h>
#include <WPointF.h>

MySeat::MySeat(WCompositor *compositor):WSeat(compositor)
{
    // To avoid parsing to MyCompositor every time
    comp = (MyCompositor*)compositor;
}

void MySeat::setCursorRequest(WSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY)
{
    if(cursorSurface)
        comp->cursor->setTexture(cursorSurface->texture(),WPointF(hotspotX,hotspotY));
    else
        comp->cursor->setTexture(comp->defaultCursorTexture,WPointF(0,0));
}

void MySeat::pointerMoveEvent(float dx, float dy)
{

    // Update the cursor position
    comp->cursor->move(dx,dy);

    if(!comp->cursor->hasHardwareSupport())
        comp->repaintAllOutputs();

    float x = comp->cursor->position().x();
    float y = comp->cursor->position().y();


    // Resizing
    if(resizingSurface)
    {
        WSize newSize = resizingSurface->calculateResizeRect(
                    resizeInitMousePos-comp->cursor->position(),
                    resizeInitSurfaceRect.bottomRight(),
                    resizeEdge);

        resizingSurface->sendConfigureToplevelEvent(
                    newSize.w(),
                    newSize.h(),
                    SurfaceState::Activated | SurfaceState::Resizing);
        return;
    }

    // Moving surface
    if(movingSurface)
    {
        movingSurface->setXWithoutDecoration(movingSurfaceInitPos.x() + int( x - movingSurfaceInitCursorPos.x() ));
        movingSurface->setYWithoutDecoration(movingSurfaceInitPos.y() + int( y - movingSurfaceInitCursorPos.y() ));
        comp->repaintAllOutputs();
        return;
    }


    // Find the surface at cursor positon
    MySurface *surface = comp->surfaceAt(comp->cursor->position());

    // If there is a surface with pointer focus
    if(pointerFocus())
    {
        bool cursorIsOverFocusSurface = surface == pointerFocus();

        // Send pointer move event if cursor is over, or if the pointer is dragging
        if(cursorIsOverFocusSurface || isLeftMouseButtonPressed)
        {
            surface->pointer().sendMotionEvent(surface->mapXtoLocal(x),surface->mapYtoLocal(y));
            return;
        }

        // Unset focus if the cursor is not over the surface and its not being dragged
        if(!cursorIsOverFocusSurface && !isLeftMouseButtonPressed)
        {
            pointerFocus()->pointer().unsetFocus();
            comp->cursor->setTexture(comp->defaultCursorTexture,WPointF(0,0));
        }
    }

    // Check if a surface was actually found
    if(surface)
    {
        if(surface->type() == SurfaceType::Undefined || surface->type() == SurfaceType::Cursor) return;

        // Set the focus
        surface->pointer().setFocus(surface->mapXtoLocal(x),surface->mapXtoLocal(y));
    }

}


void MySeat::pointerClickEvent(UInt32 button, UInt32 state)
{

    MySurface *surface = (MySurface*)pointerFocus();

    if(surface)
    {
        surface->pointer().sendButtonEvent(button,state);

        if(state == LIBINPUT_BUTTON_STATE_PRESSED && button == LEFT_BUTTON)
        {
            isLeftMouseButtonPressed = true;

            surface->keyboard().setFocus();

            // Raise view
            if(surface->parent() == nullptr)
                comp->riseSurface((MySurface*)surface);
            else
                comp->riseSurface((MySurface*)surface->topParent());

        }
    }

    if(state == LIBINPUT_BUTTON_STATE_RELEASED && button == LEFT_BUTTON)
    {
        isLeftMouseButtonPressed = false;

        // Send a mouse leave event if the surface was being dragged
        if(surface)
        {
            if(!surface->inputRegionContainsPoint(surface->pos,comp->cursor->position()))
            {
                surface->pointer().unsetFocus();
                comp->cursor->setTexture(comp->defaultCursorTexture,WPointF(0,0));
            }
        }

        if(resizingSurface)
        {
            resizingSurface->sendConfigureToplevelEvent(
                        resizingSurface->getRectWithoutDecoration().w(),
                        resizingSurface->getRectWithoutDecoration().h(),
                        SurfaceState::Activated);
            resizingSurface = nullptr;
        }
        movingSurface = nullptr;
    }




}

void MySeat::keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    if(keyboardFocus())
        keyboardFocus()->keyboard().sendKeyModifiersEvent(depressed,latched,locked,group);
}

void MySeat::keyEvent(UInt32 keyCode, UInt32 keyState)
{
    if(keyboardFocus())
        keyboardFocus()->keyboard().sendKeyEvent(keyCode,keyState);

    printf("Key:%i State:%i\n",keyCode,keyState);

    if(keyState == LIBINPUT_KEY_STATE_RELEASED)
    {
        // Ends compositor if ESC is pressed
        if(keyCode == 1)
        {
            WWayland::terminateDisplay();
            exit(0);
        }
        if(keyCode == 2)
        {
            pid_t pid = fork();
            if (pid==0)
            {
                system("weston-terminal");
                exit(0);
            }
        }
        if(keyCode == 3)
        {
            pid_t pid = fork();
            if (pid==0)
            {
                system("falkon --platform wayland");
                exit(0);
            }
        }
        if(keyCode == 4)
        {
            pid_t pid = fork();
            if (pid==0)
            {
                system("/home/eduardo/Escritorio/build-menus-Desktop_Qt_6_3_0_GCC_64bit-Debug/menus --platform wayland");
                exit(0);
            }
        }
    }

}

#include "MySeat.h"
#include "MyCompositor.h"
#include <WCursor.h>
#include <WWayland.h>
#include <unistd.h>
#include <WPoint.h>

MySeat::MySeat():WSeat()
{

}

void MySeat::setCursorRequest(WSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY)
{

    MyCompositor *comp = (MyCompositor*)compositor();

    if(cursorSurface->isDamaged())
        cursorSurface->applyDamages();
    cursorSurface->requestNextFrame();


    if(cursorSurface)
    {
        comp->cursor->setTexture(cursorSurface->texture(),WPointF(hotspotX,hotspotY));
    }
    else
    {
        comp->cursor->setTexture(comp->defaultCursorTexture,WPointF(0,0));
    }
}

void MySeat::pointerMoveEvent(float dx, float dy)
{
    MyCompositor *comp = (MyCompositor*)compositor();
    MySurface *surface;
    comp->cursor->move(dx,dy);

    float x = comp->cursor->position().x();
    float y = comp->cursor->position().y();

    if(!comp->cursor->hasHardwareSupport())
        comp->repaintAllOutputs();

    // Resizing
    if(resizingSurface != nullptr)
    {
        WSize newSize = resizingSurface->calculateResizeRect(
                    resizeInitMousePos-WPoint(comp->cursor->position()),
                    resizeInitSurfaceRect.bottomRight(),
                    resizeEdge);

        resizingSurface->sendConfigureToplevelEvent(
                    newSize.w(),
                    newSize.h(),
                    SurfaceState::Activated | SurfaceState::Resizing);
        return;
    }

    // Moving surface
    if(movingSurface != nullptr)
    {
        movingSurface->setXWithoutDecoration(movingSurfaceInitPos.x() + int( x - movingSurfaceInitCursorPos.x() ));
        movingSurface->setYWithoutDecoration(movingSurfaceInitPos.y() + int( y - movingSurfaceInitCursorPos.y() ));
        comp->repaintAllOutputs();
        return;
    }

    for(list<MySurface*>::reverse_iterator s = comp->surfacesList.rbegin(); s != comp->surfacesList.rend(); s++)
    {

        surface =*s;

        if(surface->type() == SurfaceType::Undefined) continue;

        // Mouse move event
        if(surface->containsPoint(x,y,false) && pointerFocusSurface == surface)
        {
            surface->sendPointerMotionEvent(surface->mapXtoLocal(x),surface->mapYtoLocal(y),comp->getMilliseconds());
            break;
        }

        // Mouse leave surface
        if(!surface->containsPoint(x,y,false) && pointerFocusSurface == surface)
        {
            pointerFocusSurface->sendPointerLeaveEvent();
            pointerFocusSurface = nullptr;
            cursorSurface = nullptr;
            comp->cursor->setTexture(comp->defaultCursorTexture,WPointF(0,0));
            printf("Mouse left surface\n");
            continue;
        }

        // Mouse enter surface
        if(surface->containsPoint(x,y,false) && pointerFocusSurface != surface)
        {

            if(pointerFocusSurface != nullptr)
                pointerFocusSurface->sendPointerLeaveEvent();

            surface->sendPointerEnterEvent(surface->mapXtoLocal(x),surface->mapXtoLocal(y));

            pointerFocusSurface = surface;

            printf("Mouse entered surface\n");

            break;
        }
    }

}


void MySeat::pointerClickEvent(UInt32 button, UInt32 state)
{

    MyCompositor *comp = (MyCompositor*)compositor();

    if(state == LIBINPUT_BUTTON_STATE_RELEASED && button == 272)
    {
        isLeftMouseButtonPressed = false;

        if(resizingSurface != nullptr)
        {
            resizingSurface->sendConfigureToplevelEvent(
                        resizingSurface->getRectWithoutDecoration().w(),
                        resizingSurface->getRectWithoutDecoration().h(),
                        SurfaceState::Activated);
            resizingSurface = nullptr;
        }
        movingSurface = nullptr;
    }


    if(pointerFocusSurface != nullptr)
    {
        pointerFocusSurface->sendPointerButtonEvent(button,state,comp->getMilliseconds());

        if(state == LIBINPUT_BUTTON_STATE_PRESSED && button == 272)
        {
            isLeftMouseButtonPressed = true;

            if(keyboardFocusSurface != pointerFocusSurface)
            {
                if(keyboardFocusSurface != nullptr)
                    keyboardFocusSurface->sendKeyboardLeaveEvent();

                pointerFocusSurface->sendKeyboardEnterEvent();
                keyboardFocusSurface = pointerFocusSurface;
            }

            // Raise view
            if(pointerFocusSurface->parent() == nullptr)
                comp->riseSurface((MySurface*)pointerFocusSurface);
            else
                comp->riseSurface((MySurface*)pointerFocusSurface->topParent());

        }
    }

    comp->repaintAllOutputs();
}

void MySeat::keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    if(keyboardFocusSurface)
        keyboardFocusSurface->sendKeyModifiersEvent(depressed,latched,locked,group);

    compositor()->repaintAllOutputs();
}

void MySeat::keyEvent(UInt32 keyCode, UInt32 keyState)
{
    if(keyboardFocusSurface)
    {
        keyboardFocusSurface->sendKeyEvent(keyCode,keyState);
        compositor()->repaintAllOutputs();
    }

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

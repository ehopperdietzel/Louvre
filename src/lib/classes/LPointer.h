#ifndef LPOINTER_H
#define LPOINTER_H

#include <LNamespaces.h>
#include <LPoint.h>
#include <LToplevelRole.h>

class Louvre::LPointer
{
public:

    enum POINTER_BUTTON : UInt32
    {
        LEFT_BUTTON = 272
    };

    LPointer(LSeat *seat);

    LSeat *seat() const;
    LCompositor *compositor() const;
    LCursor *cursor() const;
    LSurface *focusSurface() const;

    // Events
    void setFocus(LSurface *surface);
    void sendMoveEvent();
    void sendButtonEvent(UInt32 button, UInt32 state);

    // TopLevel Resizing Setters
    void startResizingToplevel(LToplevelRole *toplevel, LToplevelRole::Edge edge);
    bool updateResizingToplevelSize();
    void updateResizingToplevelPos();
    void stopResizingToplevel();

    // TopLevel Moving Setters
    void startMovingTopLevel(LToplevelRole *topLevel);
    bool updateMovingTopLevelPos();
    void stopMovingTopLevel();

    // Dragging Surface Setter
    void setDragginSurface(LSurface *surface);

    // Cursor Surface Setter
    void setCursorSurface(LSurface *surface);

    // Popups
    void dismissPopups();

    LSurface *draggingSurface() const;
    LSurface *cursorSurface() const;
    LToplevelRole *resizingToplevel() const;
    LToplevelRole *movingTopLevel() const;

    // TopLevel Moving Getters
    const LPoint &movingTopLevelInitPos() const;
    const LPoint &movingTopLevelInitCursorPos() const;

    // TopLevel Resizing Getters
    const LPoint &resizingToplevelInitPos() const;
    const LPoint &resizingToplevelInitCursorPos() const;
    const LSize &resizingToplevelInitSize() const;
    LToplevelRole::Edge resizingToplevelEdge() const;

    LSurface *surfaceAt(const LPoint &point, bool useRolePos = false);


private:
    friend class Globals::Surface;
    friend class Extensions::XdgShell::Toplevel;

    // Events
    void sendLeaveEvent(LSurface *surface);
    void sendEnterEvent(LSurface *surface, const LPoint &point);

    // Wayland
    LSeat               *p_seat                     = nullptr;
    LSurface            *p_pointerFocusSurface      = nullptr;
    LSurface            *p_draggingSurface          = nullptr;
    LSurface            *p_cursorSurface            = nullptr;
    LToplevelRole       *p_movingTopLevel           = nullptr;
    LToplevelRole       *p_resizingToplevel         = nullptr;

    // TopLevel Moving
    LPoint               p_movingTopLevelInitPos;
    LPoint               p_movingTopLevelInitCursorPos;

    // Resizing
    LPoint               p_resizingToplevelInitPos;
    LPoint               p_resizingToplevelInitCursorPos;
    LSize                p_resizingToplevelInitSize;
    LSize                p_resizingToplevelInitWindowSize;
    LToplevelRole::Edge  p_resizingToplevelEdge;

protected:

    friend class Louvre::LSeat;
    friend class Globals::Pointer;

    /* Libinput events */
    virtual void pointerMoved(float dx, float dy);
    virtual void pointerButtonChanged(UInt32 button, UInt32 state);

    /* Wayland requests */
    virtual void setCursorRequest(LSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY);

};


#endif // LPOINTER_H

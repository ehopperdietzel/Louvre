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

    enum POINTER_BUTTON_STATE : UInt32
    {
        RELEASED = 0,
        PRESSED = 1
    };

    enum POINTER_AXIS_SOURCE : UInt32
    {
        WHEEL = 0,
        FINGER = 1,
        CONTINUOUS = 2,
#if LOUVRE_SEAT_VERSION >= 6
        WHEEL_TILT = 3
#endif
    };

    LPointer(LSeat *seat);

    LSeat *seat() const;
    LCompositor *compositor() const;
    LCursor *cursor() const;
    LSurface *focusSurface() const;

    // Events
    void setFocus(LSurface *surface, bool useRolePos = false);
    void setFocus(LSurface *surface, const LPoint &pos);
    void sendMoveEvent(bool useRolePos = false);
    void sendMoveEvent(const LPoint &pos);
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

    // Axis
#if LOUVRE_SEAT_VERSION >= 5
    void sendAxisEvent(double x, double y, UInt32 source);
    const LPoint &scrollWheelStep() const;
    void setScrollWheelStep(const LPoint &step);
#else
    void sendAxisEvent(double x, double y);
#endif

    LSurface *surfaceAt(const LPoint &point, bool useRolePos = false);


private:
    friend class Globals::Surface;
    friend class Extensions::XdgShell::Toplevel;

    // Events
    void sendLeaveEvent(LSurface *surface);
    void sendEnterEvent(LSurface *surface, const LPoint &point);

    // Wayland
    LSeat               *m_seat                     = nullptr;
    LSurface            *m_pointerFocusSurface      = nullptr;
    LSurface            *m_draggingSurface          = nullptr;
    LSurface            *m_cursorSurface            = nullptr;
    LToplevelRole       *m_movingTopLevel           = nullptr;
    LToplevelRole       *m_resizingToplevel         = nullptr;

    // TopLevel Moving
    LPoint               m_movingTopLevelInitPos;
    LPoint               m_movingTopLevelInitCursorPos;

    // Resizing
    LPoint               m_resizingToplevelInitPos;
    LPoint               m_resizingToplevelInitCursorPos;
    LSize                m_resizingToplevelInitSize;
    LSize                m_resizingToplevelInitWindowSize;
    LToplevelRole::Edge  m_resizingToplevelEdge;

    // Axis
#if LOUVRE_SEAT_VERSION >= 5
    LPoint               m_axisDiscreteStep = LPoint(15,15);
#endif
protected:

    friend class Louvre::LSeat;
    friend class Globals::Pointer;

    /* Libinput events */
    virtual void pointerMoveEvent(float dx, float dy);
    virtual void pointerButtonEvent(UInt32 button, UInt32 state);
#if LOUVRE_SEAT_VERSION >= 5
    virtual void pointerAxisEvent(double x, double y, UInt32 source);
#else
    virtual void pointerAxisEvent(double x, double y);
#endif
    /* Wayland requests */
    virtual void setCursorRequest(LSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY);

};


#endif // LPOINTER_H

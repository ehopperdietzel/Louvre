#ifndef LSEAT_H
#define LSEAT_H

#include <LNamespaces.h>

#include <xkbcommon/xkbcommon.h>
#include <LPoint.h>
#include <LToplevelRole.h>
#include <LSurface.h>

class Louvre::LSeat
{

public:

    enum POINTER_BUTTON : UInt32
    {
        LEFT_BUTTON = 272
    };

    /* Configuration */
    LSeat(LCompositor *compositor);
    virtual ~LSeat();
    void setKeymap(const char *rules = NULL, const char *model = NULL, const char *layout = NULL, const char *variant = NULL, const char *options = NULL);
    Int32 keymapFd()    const {return p_xkbKeymapFd;}
    Int32 keymapSize()  const {return p_xkbKeymapSize;}
    Int32 libinputFd()  const;
    LCompositor *compositor() const;
    LCursor *cursor() const;

    // Pointer
    void setPointerFocus(LSurface *surface);
    void sendPointerMoveEvent();
    void sendPointerButtonEvent(UInt32 button, UInt32 state);
    LSurface *pointerFocusSurface() const;

    // Keyboard
    void setKeyboardFocus(LSurface *surface);
    void sendKeyboardKeyEvent(UInt32 keyCode, UInt32 keyState) const;
    void sendKeyboardModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) const;
    void sendKeyboardModifiersEvent() const;

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

    LSurface *keyboardFocusSurface() const;
    LSurface *touchFocusSurface() const;
    LSurface *draggingSurface() const;
    LSurface *cursorSurface() const;
    LToplevelRole *resizingToplevel() const;
    LToplevelRole *movingTopLevel() const;
    LToplevelRole *activeTopLevel() const;

    // TopLevel Moving Getters
    const LPoint &movingTopLevelInitPos() const;
    const LPoint &movingTopLevelInitCursorPos() const;

    // TopLevel Resizing Getters
    const LPoint &resizingToplevelInitPos() const;
    const LPoint &resizingToplevelInitCursorPos() const;
    const LSize &resizingToplevelInitSize() const;
    LToplevelRole::Edge resizingToplevelEdge() const;

    LSurface *surfaceAt(const LPoint &point, LSurface::PosMode mode = LSurface::NoRole);
    xkb_keysym_t keySymbol(UInt32 keyCode);

protected:

    friend class LSurface;
    friend class LToplevelRole;
    friend class Globals::Surface;
    friend class Globals::Pointer;

    /* Libinput events */
    virtual void libinputEvent(libinput_event *ev){(void)ev;}
    virtual void pointerMoveEvent(float dx, float dy);
    virtual void pointerClickEvent(UInt32 button, UInt32 state);
    virtual void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group);
    virtual void keyEvent(UInt32 keyCode,UInt32 keyState);

    /* Wayland requests */
    virtual void setCursorRequest(LSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY);


private:

    friend class LWayland;
    friend class LCompositor;
    friend class LPointer;
    friend class LKeyboard;
    friend class Extensions::XdgShell::Toplevel;

    // Events
    void sendPointerLeaveEvent(LSurface *surface);
    void sendPointerEnterEvent(LSurface *surface, const LPoint &point);

    // Wayland++
    LCompositor         *p_compositor               = nullptr;
    LSurface            *p_pointerFocusSurface      = nullptr;
    LSurface            *p_keyboardFocusSurface     = nullptr;
    LSurface            *p_touchFocusSurface        = nullptr;
    LSurface            *p_draggingSurface          = nullptr;
    LSurface            *p_cursorSurface            = nullptr;
    LToplevelRole       *p_movingTopLevel           = nullptr;
    LToplevelRole       *p_resizingToplevel         = nullptr;
    LToplevelRole       *p_activeTopLevel           = nullptr;
    wl_array             p_keys;

    // TopLevel Moving
    LPoint               p_movingTopLevelInitPos;
    LPoint               p_movingTopLevelInitCursorPos;

    // Resizing
    LPoint               p_resizingToplevelInitPos;
    LPoint               p_resizingToplevelInitCursorPos;
    LSize                p_resizingToplevelInitSize;
    LSize                p_resizingToplevelInitWindowSize;
    LToplevelRole::Edge  p_resizingToplevelEdge;


    // Libinput
    libinput            *p_li                       = nullptr;
    udev                *p_udev                     = nullptr;
    libinput_interface   p_libinputInterface;

    void processInput();
    static int openRestricted(const char *path, int flags, void *userData);
    static void closeRestricted(int fd, void *userData);

    // XKB
    xkb_context         *p_xkbContext               = nullptr;
    xkb_keymap          *p_xkbKeymap                = nullptr;
    xkb_state           *p_xkbKeymapState           = nullptr;
    xkb_rule_names       p_xkbKeymapName;
    Int32                p_xkbKeymapSize;
    Int32                p_xkbKeymapFd;

    void updateModifiers();

    struct ModifiersState
    {
        UInt32 depressed;
        UInt32 latched;
        UInt32 locked;
        UInt32 group;
    }p_modifiersState;




};

#endif // LSEAT_H

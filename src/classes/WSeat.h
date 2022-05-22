#ifndef WSEAT_H
#define WSEAT_H

#include <WNamespaces.h>

#include <xkbcommon/xkbcommon.h>
#include <WPoint.h>
#include <WToplevelRole.h>

class Wpp::WSeat
{
public:

    /* Configuration */
    WSeat(WCompositor *compositor);
    virtual ~WSeat();
    void setKeymap(const char *rules = NULL, const char *model = NULL, const char *layout = NULL, const char *variant = NULL, const char *options = NULL);
    Int32 keymapFd()    const {return p_xkbKeymapFd;}
    Int32 keymapSize()  const {return p_xkbKeymapSize;}
    Int32 libinputFd()  const;
    WCompositor *compositor() const;
    WCursor *cursor() const;


    // Pointer
    void setPointerFocus(WSurface *surface);
    void sendPointerMoveEvent();
    void sendPointerButtonEvent(UInt32 button, UInt32 state);
    WSurface *pointerFocusSurface() const;


    // Keyboard
    void setKeyboardFocus(WSurface *surface);
    void sendKeyboardKeyEvent(UInt32 keyCode, UInt32 keyState) const;
    void sendKeyboardModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) const;
    void sendKeyboardModifiersEvent() const;

    // TopLevel Resizing Setters
    void startResizingToplevel(WToplevelRole *toplevel, WToplevelRole::Edge edge);
    bool updateResizingToplevelSize();
    void updateResizingToplevelPos();
    void stopResizingToplevel();

    // TopLevel Moving Setters
    void startMovingTopLevel(WToplevelRole *topLevel);
    bool updateMovingTopLevelPos();
    void stopMovingTopLevel();

    // Dragging Surface Setter
    void setDragginSurface(WSurface *surface);

    // Cursor Surface Setter
    void setCursorSurface(WSurface *surface);

    WSurface *keyboardFocusSurface() const;
    WSurface *touchFocusSurface() const;
    WSurface *draggingSurface() const;
    WSurface *cursorSurface() const;
    WToplevelRole *resizingToplevel() const;
    WToplevelRole *movingTopLevel() const;
    WToplevelRole *activeTopLevel() const;

    // TopLevel Moving Getters
    const WPoint &movingTopLevelInitPos() const;
    const WPoint &movingTopLevelInitCursorPos() const;

    // TopLevel Resizing Getters
    const WPoint &resizingToplevelInitPos() const;
    const WPoint &resizingToplevelInitCursorPos() const;
    const WSize &resizingToplevelInitSize() const;
    WToplevelRole::Edge resizingToplevelEdge() const;

    WSurface *surfaceAt(const WPoint &point);
    xkb_keysym_t keySymbol(UInt32 keyCode);

protected:

    friend class WSurface;
    friend class WToplevelRole;
    friend class Globals::Surface;
    friend class Globals::Pointer;

    /* Libinput events */
    virtual void libinputEvent(libinput_event *ev){(void)ev;}
    virtual void pointerMoveEvent(float dx, float dy);
    virtual void pointerClickEvent(UInt32 button, UInt32 state);
    virtual void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group);
    virtual void keyEvent(UInt32 keyCode,UInt32 keyState);

    /* Wayland requests */
    virtual void setCursorRequest(WSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY);


private:

    friend class WWayland;
    friend class WCompositor;
    friend class WPointer;
    friend class WKeyboard;

    // Events
    void sendPointerLeaveEvent(WSurface *surface);
    void sendPointerEnterEvent(WSurface *surface, const WPoint &point);

    // Wayland++
    WCompositor         *p_compositor               = nullptr;
    WSurface            *p_pointerFocusSurface      = nullptr;
    WSurface            *p_keyboardFocusSurface     = nullptr;
    WSurface            *p_touchFocusSurface        = nullptr;
    WSurface            *p_draggingSurface          = nullptr;
    WSurface            *p_cursorSurface            = nullptr;
    WToplevelRole       *p_movingTopLevel           = nullptr;
    WToplevelRole       *p_resizingToplevel         = nullptr;
    WToplevelRole       *p_activeTopLevel           = nullptr;
    wl_array             p_keys;

    // TopLevel Moving
    WPoint               p_movingTopLevelInitPos;
    WPoint               p_movingTopLevelInitCursorPos;

    // Resizing
    WPoint               p_resizingToplevelInitPos;
    WPoint               p_resizingToplevelInitCursorPos;
    WSize                p_resizingToplevelInitSize;
    WSize                p_resizingToplevelInitWindowSize;
    WToplevelRole::Edge  p_resizingToplevelEdge;


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

#endif // WSEAT_H

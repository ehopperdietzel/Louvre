#ifndef WSEAT_H
#define WSEAT_H

#include <WNamespaces.h>

#include <xkbcommon/xkbcommon.h>

class Wpp::WSeat
{
public:

    /* Configuration */
    WSeat(WCompositor *compositor);
    void setKeymap(const char *rules = NULL, const char *model = NULL, const char *layout = NULL, const char *variant = NULL, const char *options = NULL);
    Int32 keymapFd()    const {return p_xkbKeymapFd;}
    Int32 keymapSize()  const {return p_xkbKeymapSize;}
    Int32 libinputFd()  const;
    WCompositor *compositor() const {return p_compositor;}
    WSurface *pointerFocus() const;
    WSurface *keyboardFocus() const;
    WSurface *touchFocus() const;

protected:

    friend class WSurface;
    friend class Globals::Surface;
    friend class Globals::Pointer;

    /* Libinput events */
    virtual void libinputEvent(libinput_event *ev){(void)ev;}
    virtual void pointerMoveEvent(float dx, float dy){(void)dx;(void)dy;}
    virtual void pointerClickEvent(UInt32 button, UInt32 state){(void)button;(void)state;}
    virtual void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group){(void)depressed;(void)latched;(void)locked;(void)group;}
    virtual void keyEvent(UInt32 keyCode,UInt32 keyState){(void)keyCode;(void)keyState;}

    /* Wayland requests */
    virtual void setCursorRequest(WSurface *cursorSurface, Int32 hotspotX, Int32 hotspotY){(void)cursorSurface;(void)hotspotX;(void)hotspotY;}


private:

    friend class WWayland;
    friend class WCompositor;
    friend class WPointer;
    friend class WKeyboard;

    // Wayland++
    WCompositor         *p_compositor               = nullptr;
    WSurface            *p_pointerFocusSurface      = nullptr;
    WSurface            *p_keyboardFocusSurface     = nullptr;
    WSurface            *p_touchFocusSurface        = nullptr;


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

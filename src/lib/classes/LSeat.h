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

    enum CAPABILITIES : UInt32
    {
        POINTER = 1,
        KEYBOARD = 2,
        TOUCH = 4
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
    UInt32 capabilities() const;
    void setCapabilities(UInt32 capabilitiesFlags);

    LSurface *keyboardFocusSurface() const;
    LSurface *touchFocusSurface() const;
    LToplevelRole *activeTopLevel() const;

    // Keyboard
    void setKeyboardFocus(LSurface *surface);
    void sendKeyboardKeyEvent(UInt32 keyCode, UInt32 keyState) const;
    void sendKeyboardModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) const;
    void sendKeyboardModifiersEvent() const;


    xkb_keysym_t keySymbol(UInt32 keyCode);


    LPointer *pointer() const;

protected:

    friend class LSurface;
    friend class LToplevelRole;
    friend class Globals::Surface;
    friend class Globals::Pointer;

    /* Libinput events */
    virtual void libinputEvent(libinput_event *ev){(void)ev;}
    virtual void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group);
    virtual void keyEvent(UInt32 keyCode,UInt32 keyState);


private:

    friend class LWayland;
    friend class LCompositor;
    friend class LPointer;
    friend class LKeyboard;
    friend class Extensions::XdgShell::Toplevel;

    // Wayland
    LCompositor         *p_compositor               = nullptr;
    LPointer            *p_pointer                  = nullptr;
    LSurface            *p_keyboardFocusSurface     = nullptr;
    LSurface            *p_touchFocusSurface        = nullptr;
    LToplevelRole       *p_activeTopLevel           = nullptr;
    wl_array             p_keys;
    UInt32               p_capabilities             = POINTER | KEYBOARD;

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

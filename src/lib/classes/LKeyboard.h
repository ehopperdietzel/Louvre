#ifndef LKEYBOARD_H
#define LKEYBOARD_H

#include <LNamespaces.h>
#include <xkbcommon/xkbcommon.h>

class Louvre::LKeyboard
{
public:
    LKeyboard(LSeat *seat);
    virtual ~LKeyboard();
    LSeat *seat() const;
    LCompositor *compositor() const;

    void setKeymap(const char *rules = NULL, const char *model = NULL, const char *layout = NULL, const char *variant = NULL, const char *options = NULL);
    Int32 keymapFd() const;
    Int32 keymapSize() const;


    LSurface *focusSurface() const;


    // Keyboard
    void setFocus(LSurface *surface);
    void sendKeyEvent(UInt32 keyCode, UInt32 keyState);
    void sendModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group);
    void sendModifiersEvent();

    xkb_keysym_t keySymbol(UInt32 keyCode);

protected:
    virtual void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group);
    virtual void keyEvent(UInt32 keyCode,UInt32 keyState);

private:

    friend class LSeat;
    friend class Globals::Surface;

    LSeat *p_seat = nullptr;

    // Wayland
    LSurface            *p_keyboardFocusSurface     = nullptr;
    wl_array             p_keys;

    // XKB
    xkb_context         *p_xkbContext               = nullptr;
    xkb_keymap          *p_xkbKeymap                = nullptr;
    xkb_state           *p_xkbKeymapState           = nullptr;
    xkb_rule_names       p_xkbKeymapName;
    Int32                p_xkbKeymapSize;
    Int32                p_xkbKeymapFd              = -1;

    void updateModifiers();

    struct ModifiersState
    {
        UInt32 depressed;
        UInt32 latched;
        UInt32 locked;
        UInt32 group;
    }p_modifiersState;
};

#endif // LKEYBOARD_H

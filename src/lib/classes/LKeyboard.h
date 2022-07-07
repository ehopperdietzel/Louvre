#ifndef LKEYBOARD_H
#define LKEYBOARD_H

#include <LNamespaces.h>
#include <xkbcommon/xkbcommon.h>

class Louvre::LKeyboard
{
public:

    struct LKeyboardModifiersState
    {
        UInt32 depressed;
        UInt32 latched;
        UInt32 locked;
        UInt32 group;
    };

    enum LKeyState : UChar8
    {
        Released = 0,
        Pressed = 1
    };

    LKeyboard(LSeat *seat);
    virtual ~LKeyboard();
    LSeat *seat() const;
    LCompositor *compositor() const;
    LSurface *focusSurface() const;
    const LKeyboardModifiersState &modifiersState() const;

    Int32 keymapFd() const;
    Int32 keymapSize() const;

#if LOUVRE_SEAT_VERSION >= 4
    Int32 repeatRate() const;
    Int32 repeatDelay() const;
    void setRepeatInfo(Int32 rate, Int32 msDelay);
#endif

    xkb_keysym_t keySymbol(UInt32 keyCode);

    void setKeymap(const char *rules = NULL, const char *model = NULL, const char *layout = NULL, const char *variant = NULL, const char *options = NULL);



    // Events
    void setFocus(LSurface *surface);
    void sendKeyEvent(UInt32 keyCode, UInt32 keyState);
    void sendModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group);
    void sendModifiersEvent();



protected:
    virtual void keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group);
    virtual void keyEvent(UInt32 keyCode,UInt32 keyState);

private:

    friend class LSeat;
    friend class Globals::Surface;

    LSeat *m_seat = nullptr;

    // Wayland
    LSurface            *m_keyboardFocusSurface     = nullptr;

#if LOUVRE_SEAT_VERSION >= 4
    Int32                m_repeatRate               = 32;
    Int32                m_repeatDelay              = 500;
#endif

    wl_array             m_keys;

    // XKB
    xkb_context         *m_xkbContext               = nullptr;
    xkb_keymap          *m_xkbKeymap                = nullptr;
    xkb_state           *m_xkbKeymapState           = nullptr;
    xkb_rule_names       m_xkbKeymapName;
    Int32                m_xkbKeymapSize;
    Int32                m_xkbKeymapFd              = -1;

    void updateModifiers();

    LKeyboardModifiersState m_modifiersState;
};

#endif // LKEYBOARD_H

#include "LKeyboard.h"

#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <LTime.h>
#include <LWayland.h>
#include <LCompositor.h>
#include <LSeat.h>
#include <LClient.h>

using namespace Louvre;

LKeyboard::LKeyboard(LSeat *seat)
{
    p_seat = seat;

    // Create null keys
    wl_array_init(&p_keys);

    // Create XKB context
    p_xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    // Set the default keymap
    setKeymap();
}

LKeyboard::~LKeyboard()
{
    wl_array_release(&p_keys);
}

LSeat *LKeyboard::seat() const
{
    return p_seat;
}

LCompositor *LKeyboard::compositor() const
{
    return seat()->compositor();
}

void LKeyboard::setKeymap(const char *rules, const char *model, const char *layout, const char *variant, const char *options)
{
    if(p_xkbKeymapFd != -1)
    {
         close(p_xkbKeymapFd);
    }

    char *keymapString,*map;

    const char *xdgRuntimeDir = "/run/user/1000";

    p_xkbKeymapName.rules = rules;
    p_xkbKeymapName.model = model;
    p_xkbKeymapName.layout = layout;
    p_xkbKeymapName.variant = variant;
    p_xkbKeymapName.options = options;

    // Find a keymap matching suggestions
    p_xkbKeymap = xkb_keymap_new_from_names(p_xkbContext, &p_xkbKeymapName, XKB_KEYMAP_COMPILE_NO_FLAGS);

    // Get the keymap string
    keymapString = xkb_keymap_get_as_string(p_xkbKeymap, XKB_KEYMAP_FORMAT_TEXT_V1);

    // Store the keymap size
    p_xkbKeymapSize = strlen(keymapString) + 1;

    // Get the XDG_RUNTIME_DIR env
    //xdgRuntimeDir = "/run/user/1000/louvre_keymap";//getenv("XDG_RUNTIME_DIR");

    // Open and store the file descritor
    p_xkbKeymapFd = open(xdgRuntimeDir, O_TMPFILE|O_RDWR|O_EXCL, 0600);

    if(p_xkbKeymapFd < 0)
    {
        printf("Error creating shared memory for keyboard layout.\n");
        exit(-1);
    }

    // Write the keymap string
    ftruncate(p_xkbKeymapFd, p_xkbKeymapSize);
    map = (char*)mmap(NULL, p_xkbKeymapSize, PROT_READ|PROT_WRITE, MAP_SHARED, p_xkbKeymapFd, 0);
    //strcpy(map, keymapString);
    memcpy(map,keymapString,p_xkbKeymapSize);
    munmap(map, p_xkbKeymapSize);

    // Keymap string not needed anymore
    free(keymapString);

    // Create a xkb keyboard state to handle modifiers
    p_xkbKeymapState = xkb_state_new(p_xkbKeymap);

    for(LClient *lClient : compositor()->clients())
    {
        if(lClient->keyboardResource())
            wl_keyboard_send_keymap(lClient->keyboardResource(),WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, keymapFd(), keymapSize());
    }
}

Int32 LKeyboard::keymapFd() const
{
    return p_xkbKeymapFd;
}

Int32 LKeyboard::keymapSize() const
{
    return p_xkbKeymapSize;
}

LSurface *LKeyboard::focusSurface() const
{
    return p_keyboardFocusSurface;
}

const LKeyboard::LKeyboardModifiersState &LKeyboard::modifiersState() const
{
    return p_modifiersState;
}

void LKeyboard::setFocus(LSurface *surface)
{
    // If surface is not nullptr
    if(surface)
    {
        // If already has focus
        if(focusSurface() == surface)
            return;
        else
        {
            // If another surface has focus
            if(focusSurface() && focusSurface()->client()->keyboardResource())
            {
                focusSurface()->client()->p_lastKeyboardLeaveEventSerial = LWayland::nextSerial();
                wl_keyboard_send_leave(focusSurface()->client()->keyboardResource(),focusSurface()->client()->p_lastKeyboardLeaveEventSerial,focusSurface()->resource());
            }

            if(surface->client()->keyboardResource())
            {
                surface->client()->p_lastKeyboardEnterEventSerial = LWayland::nextSerial();
                wl_keyboard_send_enter(surface->client()->keyboardResource(),surface->client()->p_lastKeyboardEnterEventSerial,surface->resource(),&p_keys);
                p_keyboardFocusSurface = surface;
                sendModifiersEvent();
            }
            else
                p_keyboardFocusSurface = nullptr;
        }

    }
    else
    {
        // If a surface has focus
        if(focusSurface() && focusSurface()->client()->keyboardResource())
        {
            focusSurface()->client()->p_lastKeyboardLeaveEventSerial = LWayland::nextSerial();
            wl_keyboard_send_leave(focusSurface()->client()->keyboardResource(),focusSurface()->client()->p_lastKeyboardLeaveEventSerial,focusSurface()->resource());
        }
        p_keyboardFocusSurface = nullptr;
    }
}

void LKeyboard::sendKeyEvent(UInt32 keyCode, UInt32 keyState)
{
    // If no surface has focus
    if(!focusSurface())
        return;

    // If do not have a wl_keyboard
    if(!focusSurface()->client()->keyboardResource())
        return;

    focusSurface()->client()->p_lastKeyboardKeyEventSerial = LWayland::nextSerial();
    wl_keyboard_send_key(focusSurface()->client()->keyboardResource(),focusSurface()->client()->p_lastKeyboardKeyEventSerial,LTime::ms(),keyCode,keyState);
}

void LKeyboard::sendModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    // If no surface has focus
    if(!focusSurface())
        return;

    // If do not have a wl_keyboard
    if(!focusSurface()->client()->keyboardResource())
        return;

    focusSurface()->client()->p_lastKeyboardModifiersEventSerial = LWayland::nextSerial();
    wl_keyboard_send_modifiers(focusSurface()->client()->keyboardResource(),focusSurface()->client()->p_lastKeyboardModifiersEventSerial,depressed,latched,locked,group);
}

void LKeyboard::sendModifiersEvent()
{
    sendModifiersEvent(p_modifiersState.depressed, p_modifiersState.latched, p_modifiersState.locked, p_modifiersState.group);
}

xkb_keysym_t LKeyboard::keySymbol(UInt32 keyCode)
{
    return xkb_state_key_get_one_sym(p_xkbKeymapState,keyCode+8);
}

void LKeyboard::updateModifiers()
{
    p_modifiersState.depressed = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_DEPRESSED);
    p_modifiersState.latched = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_LATCHED);
    p_modifiersState.locked = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_LOCKED);
    p_modifiersState.group = xkb_state_serialize_layout(p_xkbKeymapState, XKB_STATE_LAYOUT_EFFECTIVE);
    keyModifiersEvent(p_modifiersState.depressed,p_modifiersState.latched,p_modifiersState.locked,p_modifiersState.group);
}

#if LOUVRE_SEAT_VERSION >= 4

    Int32 LKeyboard::repeatRate() const
    {
        return p_repeatRate;
    }

    Int32 LKeyboard::repeatDelay() const
    {
        return p_repeatDelay;
    }

    void LKeyboard::setRepeatInfo(Int32 rate, Int32 msDelay)
    {
        p_repeatRate = rate;
        p_repeatDelay = msDelay;

        for(LClient *lClient : compositor()->clients())
            if(lClient->keyboardResource() && wl_resource_get_version(lClient->keyboardResource()) >= 4)
                wl_keyboard_send_repeat_info(lClient->keyboardResource(),rate,msDelay);

    }

#endif

/* VIRTUAL */

void LKeyboard::keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    sendModifiersEvent(depressed,latched,locked,group);
}

void LKeyboard::keyEvent(UInt32 keyCode, UInt32 keyState)
{
    xkb_keysym_t sym = keySymbol(keyCode);

    sendKeyEvent(keyCode,keyState);

    if(keyState == LIBINPUT_KEY_STATE_RELEASED)
    {
        // Ends compositor if ESC is pressed
        if(sym == XKB_KEY_F7)
        {
            LWayland::terminateDisplay();
            exit(0);
        }
        else if(sym == XKB_KEY_F1)
        {
            if (fork()==0)
            {
                //system("gnome-terminal");
                setsid();
                char *const envp[] = {"XDG_RUNTIME_DIR=/run/user/1000",0};
                const char *argv[64] = {"/usr/bin/weston-terminal" , NULL, NULL , NULL};
                execve(argv[0], (char **)argv, envp);
                exit(0);
            }
        }
        else if(sym == XKB_KEY_F2)
        {
            if (fork()==0)
            {
                //system("/home/eduardo/Escritorio/build-notepad-Desktop_Qt_5_15_2_GCC_64bit-Release/notepad --platform wayland");
                setsid();
                char *const envp[] = {"XDG_RUNTIME_DIR=/run/user/1000",0};
                const char *argv[64] = {"/home/eduardo/Escritorio/build-notepad-Desktop_Qt_5_15_2_GCC_64bit-Release/notepad" , "--platform", "wayland" , NULL};
                execve(argv[0], (char **)argv, envp);
                exit(0);
            }
        }
        else if(sym == XKB_KEY_F3)
        {
            if (fork()==0)
            {
                setsid();
                char *const envp[] = {"XDG_RUNTIME_DIR=/run/user/1000",0};
                const char *argv[64] = {"/usr/bin/gedit" , NULL, NULL, NULL};
                execve(argv[0], (char **)argv, envp);
                exit(0);
            }
        }
        else if (sym == XKB_KEY_F4)
        {
            if (fork()==0)
            {
                setsid();
                system("/home/eduardo/Escritorio/run_chrome.sh");
                exit(0);
            }
        }
        else if (sym == XKB_KEY_XF86AudioLowerVolume)
        {
            if (fork()==0)
            {
                system("pactl -- set-sink-volume 0 -10%");
                exit(0);
            }
        }
        else if (sym == XKB_KEY_XF86AudioRaiseVolume)
        {
            if (fork()==0)
            {
                system("pactl -- set-sink-volume 0 +10%");
                exit(0);
            }
        }
        else if(sym == XKB_KEY_Escape)
        {
            if(focusSurface() && focusSurface()->toplevel() && focusSurface()->toplevel()->fullscreen())
                focusSurface()->toplevel()->configure(focusSurface()->toplevel()->state() & ~LToplevelRole::Fullscreen);
        }
    }
}

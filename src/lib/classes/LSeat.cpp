#include "LSeat.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <libinput.h>
#include <signal.h>


#include <xkbcommon/xkbcommon-compat.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon-names.h>
#include <sys/mman.h>

#include <sys/poll.h>
#include <sys/eventfd.h>

#include <LWayland.h>
#include <LCursor.h>
#include <LToplevelRole.h>
#include <LSurface.h>
#include <LCompositor.h>
#include <LTime.h>
#include <LOutput.h>
#include <LPopupRole.h>
#include <LPointer.h>

using namespace Louvre;


/****************************** Virtual mehtods ******************************/


void LSeat::keyModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group)
{
    sendKeyboardModifiersEvent(depressed,latched,locked,group);
}

void LSeat::keyEvent(UInt32 keyCode, UInt32 keyState)
{
    xkb_keysym_t sym = keySymbol(keyCode);

    sendKeyboardKeyEvent(keyCode,keyState);

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
    }
}


/****************************** Non virtual mehtods ******************************/

LSeat::LSeat(LCompositor *compositor)
{
    p_compositor = compositor;

    p_pointer = compositor->createPointerRequest(this);

    // Create null keys
    wl_array_init(&p_keys);

    // Create XKB context
    p_xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    // Set the default keymap
    setKeymap();

    // Setup libinput
    p_udev = udev_new();
    p_libinputInterface.open_restricted = &Louvre::LSeat::openRestricted;
    p_libinputInterface.close_restricted = &Louvre::LSeat::closeRestricted;
    p_li = libinput_udev_create_context(&p_libinputInterface, NULL, p_udev);
    libinput_udev_assign_seat(p_li, "seat0");
    libinput_dispatch(p_li);
}

LSeat::~LSeat()
{
    wl_array_release(&p_keys);
}

void LSeat::setKeymap(const char *rules, const char *model, const char *layout, const char *variant, const char *options)
{
    char *keymapString,*xdgRuntimeDir,*map;

    p_xkbKeymapName.rules = rules;
    p_xkbKeymapName.model = model;
    p_xkbKeymapName.layout = "latam";
    p_xkbKeymapName.variant = variant;
    p_xkbKeymapName.options = options;

    // Find a keymap matching suggestions
    p_xkbKeymap = xkb_keymap_new_from_names(p_xkbContext, &p_xkbKeymapName, XKB_KEYMAP_COMPILE_NO_FLAGS);

    // Get the keymap string
    keymapString = xkb_keymap_get_as_string(p_xkbKeymap, XKB_KEYMAP_FORMAT_TEXT_V1);

    // Store the keymap size
    p_xkbKeymapSize = strlen(keymapString) + 1;

    // Get the XDG_RUNTIME_DIR env
    xdgRuntimeDir = getenv("XDG_RUNTIME_DIR");

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
}

Int32 LSeat::libinputFd() const
{
    return libinput_get_fd(p_li);
}

LCompositor *LSeat::compositor() const
{
    return p_compositor;
}

LCursor *LSeat::cursor() const
{
    return compositor()->cursor();
}

UInt32 LSeat::capabilities() const
{
    return p_capabilities;
}

void LSeat::setCapabilities(UInt32 capabilitiesFlags)
{
    p_capabilities = capabilitiesFlags;

    for(LClient *lClient : compositor()->clients())
    {
        if(lClient->seatResource())
            wl_seat_send_capabilities(lClient->seatResource(),p_capabilities);
    }
}


void LSeat::setKeyboardFocus(LSurface *surface)
{
    // If surface is not nullptr
    if(surface)
    {
        // If already has focus
        if(keyboardFocusSurface() == surface)
            return;
        else
        {
            // If another surface has focus
            if(keyboardFocusSurface() && keyboardFocusSurface()->client()->keyboardResource())
                wl_keyboard_send_leave(keyboardFocusSurface()->client()->keyboardResource(),LWayland::nextSerial(),keyboardFocusSurface()->resource());

            if(surface->client()->keyboardResource())
            {
                wl_keyboard_send_enter(surface->client()->keyboardResource(),LWayland::nextSerial(),surface->resource(),&p_keys);
                p_keyboardFocusSurface = surface;
                sendKeyboardModifiersEvent();
            }
            else
                p_keyboardFocusSurface = nullptr;
        }

    }
    else
    {
        // If a surface has focus
        if(keyboardFocusSurface() && keyboardFocusSurface()->client()->keyboardResource())
            wl_keyboard_send_leave(keyboardFocusSurface()->client()->keyboardResource(),LWayland::nextSerial(),keyboardFocusSurface()->resource());

        p_keyboardFocusSurface = nullptr;
    }
}

void LSeat::sendKeyboardKeyEvent(UInt32 keyCode, UInt32 keyState) const
{
    // If no surface has focus
    if(!keyboardFocusSurface())
        return;

    // If do not have a wl_keyboard
    if(!keyboardFocusSurface()->client()->keyboardResource())
        return;

    wl_keyboard_send_key(keyboardFocusSurface()->client()->keyboardResource(),LWayland::nextSerial(),LTime::ms(),keyCode,keyState);
}

void LSeat::sendKeyboardModifiersEvent(UInt32 depressed, UInt32 latched, UInt32 locked, UInt32 group) const
{
    // If no surface has focus
    if(!keyboardFocusSurface())
        return;

    // If do not have a wl_keyboard
    if(!keyboardFocusSurface()->client()->keyboardResource())
        return;

    wl_keyboard_send_modifiers(keyboardFocusSurface()->client()->keyboardResource(),LWayland::nextSerial(),depressed,latched,locked,group);
}

void LSeat::sendKeyboardModifiersEvent() const
{
    sendKeyboardModifiersEvent(p_modifiersState.depressed, p_modifiersState.latched, p_modifiersState.locked, p_modifiersState.group);
}


LSurface *LSeat::keyboardFocusSurface() const
{
    return p_keyboardFocusSurface;
}

LSurface *LSeat::touchFocusSurface() const
{
    return p_touchFocusSurface;
}

LToplevelRole *LSeat::activeTopLevel() const
{
    return p_activeTopLevel;
}



xkb_keysym_t LSeat::keySymbol(UInt32 keyCode)
{
    return xkb_state_key_get_one_sym(p_xkbKeymapState,keyCode+8);
}

LPointer *LSeat::pointer() const
{
    return p_pointer;
}

void LSeat::processInput()
{
    int ret = libinput_dispatch(p_li);

    if (ret != 0)
    {
        printf("Failed to dispatch libinput: %s\n", strerror(-ret));
        return;
    }

    libinput_event *ev;

    while ((ev = libinput_get_event(p_li)) != NULL)
    {

        libinput_event_type eventType = libinput_event_get_type(ev);


        if(eventType == LIBINPUT_EVENT_POINTER_MOTION)
        {
            libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(ev);

            double x = libinput_event_pointer_get_dx(pointerEvent);
            double y = libinput_event_pointer_get_dy(pointerEvent);

            this->pointer()->pointerMoveEvent(x,y);

            if(compositor()->cursor())
                compositor()->cursor()->move(x,y);
        }
        else if(eventType == LIBINPUT_EVENT_POINTER_BUTTON)
        {
            libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(ev);
            uint32_t button = libinput_event_pointer_get_button(pointerEvent);
            libinput_button_state state = libinput_event_pointer_get_button_state(pointerEvent);
            this->pointer()->pointerButtonEvent(button,state);
        }
        else if(eventType == LIBINPUT_EVENT_KEYBOARD_KEY)
        {
            libinput_event_keyboard *keyEv = libinput_event_get_keyboard_event(ev);
            libinput_key_state keyState = libinput_event_keyboard_get_key_state(keyEv);
            int keyCode = libinput_event_keyboard_get_key(keyEv);
            xkb_state_update_key(p_xkbKeymapState,keyCode+8,(xkb_key_direction)keyState);
            keyEvent(keyCode,keyState);
            updateModifiers();
        }
        else if(eventType == LIBINPUT_EVENT_POINTER_AXIS)
        {
            libinput_event_pointer *axisEvent = libinput_event_get_pointer_event(ev);

            double x = 0.0;
            double y = 0.0;

            if(libinput_event_pointer_has_axis(axisEvent,LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL))
                x = libinput_event_pointer_get_axis_value(axisEvent,LIBINPUT_POINTER_AXIS_SCROLL_HORIZONTAL);

            if(libinput_event_pointer_has_axis(axisEvent,LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL))
                y = libinput_event_pointer_get_axis_value(axisEvent,LIBINPUT_POINTER_AXIS_SCROLL_VERTICAL);

        #if LOUVRE_SEAT_VERSION >= 5
            UInt32 source = libinput_event_pointer_get_axis_source(axisEvent);
            pointer()->pointerAxisEvent(x,y,source);
        #else
            pointer()->pointerAxisEvent(x,y);
        #endif
        }



        libinputEvent(ev);
        libinput_event_destroy(ev);
        libinput_dispatch(p_li);
    }
}

int LSeat::openRestricted(const char *path, int flags, void *)
{
    return open(path, flags);
}

void LSeat::closeRestricted(int fd, void *)
{
    close(fd);
}

void LSeat::updateModifiers()
{
    p_modifiersState.depressed = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_DEPRESSED);
    p_modifiersState.latched = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_LATCHED);
    p_modifiersState.locked = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_LOCKED);
    p_modifiersState.group = xkb_state_serialize_layout(p_xkbKeymapState, XKB_STATE_LAYOUT_EFFECTIVE);
    keyModifiersEvent(p_modifiersState.depressed,p_modifiersState.latched,p_modifiersState.locked,p_modifiersState.group);
}



/*
 * static void init_cursors() {
    s_cursors[CursorStyle_Arrow] = XcursorLibraryLoadCursor(s_display, "arrow");
    s_cursors[CursorStyle_Ibeam] = XcursorLibraryLoadCursor(s_display, "xterm");
    s_cursors[CursorStyle_Crosshair] = XcursorLibraryLoadCursor(s_display, "crosshair");
    s_cursors[CursorStyle_ClosedHand] = XcursorLibraryLoadCursor(s_display, "hand2");
    s_cursors[CursorStyle_OpenHand] = XcursorLibraryLoadCursor(s_display, "hand2");
    s_cursors[CursorStyle_ResizeLeftRight] = XcursorLibraryLoadCursor(s_display, "sb_h_double_arrow");
    s_cursors[CursorStyle_ResizeUpDown] = XcursorLibraryLoadCursor(s_display, "sb_v_double_arrow");
    s_cursors[CursorStyle_SizeAll] = XcursorLibraryLoadCursor(s_display, "diamond_cross");
}
*/

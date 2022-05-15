#include "WSeat.h"
#include <WInput.h>
#include <WCompositor.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <libinput.h>


#include <xkbcommon/xkbcommon-compat.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon-names.h>
#include <sys/mman.h>

#include <sys/poll.h>
#include <sys/eventfd.h>

#include <WWayland.h>

using namespace Wpp;

WSeat::WSeat()
{
    // Create XKB context
    p_xkbContext = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    // Set the default keymap
    setKeymap();

    // Setup libinput
    p_udev = udev_new();
    p_libinputInterface.open_restricted = &Wpp::WSeat::openRestricted;
    p_libinputInterface.close_restricted = &Wpp::WSeat::closeRestricted;
    p_li = libinput_udev_create_context(&p_libinputInterface, NULL, p_udev);
    libinput_udev_assign_seat(p_li, "seat0");
    libinput_dispatch(p_li);
}

void WSeat::setKeymap(const char *rules, const char *model, const char *layout, const char *variant, const char *options)
{
    char *keymapString,*xdgRuntimeDir,*map;

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

Int32 WSeat::libinputFd()
{
    return libinput_get_fd(p_li);
}

void WSeat::processInput()
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
            this->pointerMoveEvent(libinput_event_pointer_get_dx(pointerEvent),libinput_event_pointer_get_dy(pointerEvent));
        }
        else if(eventType == LIBINPUT_EVENT_POINTER_BUTTON)
        {
            libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(ev);
            uint32_t button = libinput_event_pointer_get_button(pointerEvent);
            libinput_button_state state = libinput_event_pointer_get_button_state(pointerEvent);
            this->pointerClickEvent(button,state);
        }
        else if(eventType == LIBINPUT_EVENT_KEYBOARD_KEY)
        {
            libinput_event_keyboard *keyEv = libinput_event_get_keyboard_event(ev);
            libinput_key_state keyState = libinput_event_keyboard_get_key_state(keyEv);
            int keyCode = libinput_event_keyboard_get_key(keyEv);

            keyEvent(keyCode,keyState);

            xkb_state_update_key(p_xkbKeymapState,keyCode+8,(xkb_key_direction)keyState);
            updateModifiers();
        }

        libinputEvent(ev);
        libinput_event_destroy(ev);
        libinput_dispatch(p_li);
    }
}

int WSeat::openRestricted(const char *path, int flags, void *)
{
    return open(path, flags);
}

void WSeat::closeRestricted(int fd, void *)
{
    close(fd);
}

void WSeat::updateModifiers()
{
    p_modifiersState.depressed = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_DEPRESSED);
    p_modifiersState.latched = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_LATCHED);
    p_modifiersState.locked = xkb_state_serialize_mods(p_xkbKeymapState, XKB_STATE_MODS_LOCKED);
    p_modifiersState.group = xkb_state_serialize_layout(p_xkbKeymapState, XKB_STATE_LAYOUT_EFFECTIVE);
    keyModifiersEvent(p_modifiersState.depressed,p_modifiersState.latched,p_modifiersState.locked,p_modifiersState.group);
}


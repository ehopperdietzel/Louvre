/*
#include <WInput.h>
#include <WCompositor.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <libudev.h>
#include <libinput.h>

#include <xkbcommon/xkbcommon.h>
#include <xkbcommon/xkbcommon-compat.h>
#include <xkbcommon/xkbcommon-compose.h>
#include <xkbcommon/xkbcommon-keysyms.h>
#include <xkbcommon/xkbcommon-names.h>
#include <sys/mman.h>

#include <sys/poll.h>
#include <sys/eventfd.h>

#include <WWayland.h>


using namespace Wpp;

WCompositor *comp;
libinput *li;
libinput_event *ev;
udev *udev;
int keymapFD,keymapSize,keymapFDPrivate;
xkb_state *xkbState;

struct modifier_state {
    UInt32 depressed, latched, locked, group;
}modifier_state;

static int open_restricted(const char *path, int flags, void *user_data)
{
    (void)user_data;
    int fd = open(path, flags);
    return fd;
}

static void close_restricted(int fd, void *user_data)
{
    (void)user_data;
    close(fd);
}

const static struct libinput_interface interface = {
        .open_restricted = open_restricted,
        .close_restricted = close_restricted,
};

void update_modifiers()
{
    modifier_state.depressed = xkb_state_serialize_mods (xkbState, XKB_STATE_MODS_DEPRESSED);
    modifier_state.latched = xkb_state_serialize_mods (xkbState, XKB_STATE_MODS_LATCHED);
    modifier_state.locked = xkb_state_serialize_mods (xkbState, XKB_STATE_MODS_LOCKED);
    modifier_state.group = xkb_state_serialize_layout (xkbState, XKB_STATE_LAYOUT_EFFECTIVE);
    comp->keyModifiersEvent(modifier_state.depressed,modifier_state.latched,modifier_state.locked,modifier_state.group);
}
int WInput::processInput(int, unsigned int, void *)
{

    int ret = libinput_dispatch(li);

    if (ret != 0)
    {
        printf("Failed to dispatch libinput: %s\n", strerror(-ret));
        return 0;
    }

    libinput_event *ev;

    while ((ev = libinput_get_event(li)) != NULL)
    {

        libinput_event_type eventType = libinput_event_get_type(ev);


        //libinput_device_config_dwt_set_enabled(libinput_event_get_device(ev),LIBINPUT_CONFIG_DWT_DISABLED);

        if(eventType == LIBINPUT_EVENT_POINTER_MOTION)
        {
            libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(ev);
            comp->pointerMoveEvent(
                        libinput_event_pointer_get_dx(pointerEvent),
                        libinput_event_pointer_get_dy(pointerEvent));
                        // libinput_event_pointer_get_time_usec(pointerEvent));
        }
        else if(eventType == LIBINPUT_EVENT_POINTER_BUTTON)
        {
            libinput_event_pointer *pointerEvent = libinput_event_get_pointer_event(ev);

            uint32_t button = libinput_event_pointer_get_button(pointerEvent);
            libinput_button_state state = libinput_event_pointer_get_button_state(pointerEvent);

            comp->pointerClickEvent(
                        button,
                        state);
                        // libinput_event_pointer_get_time_usec(pointerEvent));
        }
        else if(eventType == LIBINPUT_EVENT_KEYBOARD_KEY)
        {
            libinput_event_keyboard *keyEvent = libinput_event_get_keyboard_event(ev);
            libinput_key_state keyState = libinput_event_keyboard_get_key_state(keyEvent);
            int keyCode = libinput_event_keyboard_get_key(keyEvent);

            comp->keyEvent(
                        keyCode,
                        keyState);

            xkb_state_update_key(xkbState,keyCode+8,(xkb_key_direction)keyState);
            update_modifiers();
        }

        comp->libinputEvent(ev);
        libinput_event_destroy(ev);
        libinput_dispatch(li);
    }

    return 0;
}

int WInput::getLibinputFd()
{
    return libinput_get_fd(li);
}

void initXKB()
{
    xkb_context *ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);

    xkb_keymap *keymap;


    struct xkb_rule_names names =
    {
        .rules = NULL,
        .model = NULL,//"pc105",
        .layout = "latam",
        .variant = NULL,//"dvorak",
        .options = NULL//"terminate:ctrl_alt_bksp"
    };


    keymap = xkb_keymap_new_from_names(ctx, &names,XKB_KEYMAP_COMPILE_NO_FLAGS);

    char *string = xkb_keymap_get_as_string(keymap, XKB_KEYMAP_FORMAT_TEXT_V1);
    keymapSize = strlen(string) + 1;
    char *xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    //printf("XDG_RUNTIME_DIR:%s\n",xdg_runtime_dir);
    //printf("KEYMAP:%s\n",string);
    keymapFD = open(xdg_runtime_dir, O_TMPFILE|O_RDWR|O_EXCL, 0600);
    if(keymapFD < 0)
    {
        printf("Error creating shared memory for keyboard layout.\n");
        exit(-1);
    }

    keymapFDPrivate = open(xdg_runtime_dir, O_TMPFILE|O_RDWR|O_EXCL, 0600);
    if(keymapFDPrivate < 0)
    {
        printf("Error creating shared memory for keyboard layout.\n");
        exit(-1);
    }

    ftruncate(keymapFD, keymapSize);
    ftruncate(keymapFDPrivate, keymapSize);

    char *map = (char*)mmap(NULL, keymapSize, PROT_READ|PROT_WRITE, MAP_SHARED, keymapFD, 0);
    strcpy(map, string);
    munmap(map, keymapSize);

    map = (char*)mmap(NULL, keymapSize, PROT_READ|PROT_WRITE, MAP_PRIVATE, keymapFDPrivate, 0);
    strcpy(map, string);
    munmap(map, keymapSize);

    free(string);
    xkbState = xkb_state_new(keymap);
}

int WInput::getKeymapFD(bool privated)
{
    if(privated)
        return keymapFDPrivate;
    else
        return keymapFD;
}
int WInput::getKeymapSize()
{
    return keymapSize;
}

int WInput::initInput(WCompositor *compositor)
{
    initXKB();
    comp = compositor;
    udev = udev_new();

    li = libinput_udev_create_context(&interface, NULL, udev);
    libinput_udev_assign_seat(li, "seat0");
    libinput_dispatch(li);
    printf("Libinput initialized.\n");

    return 0;
}
*/

#include "Seat.h"

#include <LCompositor.h>

#include <Pointer.h>
#include <Keyboard.h>
#include <LSeat.h>

#include <sys/mman.h>

static struct wl_pointer_interface pointer_implementation =
{
    &Louvre::Globals::Pointer::set_cursor,
    &Louvre::Globals::Pointer::release
};

static struct wl_keyboard_interface keyboard_implementation =
{
    &Louvre::Globals::Keyboard::release
};

static struct wl_seat_interface seat_implementation =
{
    &Louvre::Globals::Seat::get_pointer,
    &Louvre::Globals::Seat::get_keyboard,
    &Louvre::Globals::Seat::get_touch,
    &Louvre::Globals::Seat::release
};

// SEAT
void Louvre::Globals::Seat::resource_destroy(wl_resource *resource)
{
    (void)resource;
    printf("SEAT DESTROYED.\n");
    //LClient *client = (LClient*)wl_resource_get_user_data(resource);
}

void Louvre::Globals::Seat::get_pointer (wl_client *client, wl_resource *resource, UInt32 id)
{
    LClient *wClient = (LClient*)wl_resource_get_user_data(resource);

    Int32 version = wl_resource_get_version(resource);

    printf("Pointer Version: %i\n",version);
    wl_resource *pointer = wl_resource_create(client, &wl_pointer_interface, version, id); // 7
    wl_resource_set_implementation(pointer, &pointer_implementation, wClient,&Pointer::resource_destroy);
    wClient->p_pointerResource = pointer;
}

void Louvre::Globals::Seat::get_keyboard (wl_client *client, wl_resource *resource, UInt32 id)
{
    LClient *wClient = (LClient*)wl_resource_get_user_data(resource);

    Int32 version = wl_resource_get_version(resource);
    wl_resource *keyboard = wl_resource_create(client, &wl_keyboard_interface,version,id); // 7
    wl_resource_set_implementation(keyboard, &keyboard_implementation, wClient, &Keyboard::resource_destroy);
    wClient->p_keyboardResource = keyboard;
    wl_keyboard_send_repeat_info(keyboard,32,512);

    printf("Keyboard Version: %i\n",version);

    //if(version >= 7)
        //wl_keyboard_send_keymap(keyboard, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1,WInput::getKeymapFD(true),WInput::getKeymapSize());
    //else
    wl_keyboard_send_keymap(keyboard, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1,wClient->compositor()->seat()->keymapFd(),wClient->compositor()->seat()->keymapSize());

}

void Louvre::Globals::Seat::get_touch(wl_client *client, wl_resource *resource, UInt32 id)
{
    (void)client;(void)resource;(void)id;
}

void Louvre::Globals::Seat::release( wl_client *client, wl_resource *resource)
{
    (void)client;
    printf("SEAT RELEASED\n");
    Seat::resource_destroy(resource);
}

void Louvre::Globals::Seat::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    (void)version;

    LCompositor *compositor = (LCompositor*)data;

    wl_resource *seat = wl_resource_create(client, &wl_seat_interface,version,id);
    LClient *wClient = *find_if(compositor->clients.begin(),compositor->clients.end(),[client](LClient *x) { return x->client() == client;});
    wl_resource_set_implementation(seat, &seat_implementation, wClient, &Seat::resource_destroy);
    wl_seat_send_capabilities(seat, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);

}

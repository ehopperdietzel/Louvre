#include "Seat.h"

#include <WCompositor.h>

#include <Pointer.h>
#include <Keyboard.h>
#include <WInput.h>

#include <sys/mman.h>

static struct wl_pointer_interface pointer_implementation =
{
    &Wpp::Globals::Pointer::set_cursor,
    &Wpp::Globals::Pointer::release
};

static struct wl_keyboard_interface keyboard_implementation =
{
    &Wpp::Globals::Keyboard::release
};

static struct wl_seat_interface seat_implementation =
{
    &Wpp::Globals::Seat::get_pointer,
    &Wpp::Globals::Seat::get_keyboard,
    &Wpp::Globals::Seat::get_touch,
    &Wpp::Globals::Seat::release
};

// SEAT
void Wpp::Globals::Seat::resource_destroy(wl_resource *resource)
{
    (void)resource;
    printf("SEAT DESTROYED.\n");
    //WClient *client = (WClient*)wl_resource_get_user_data(resource);
}

void Wpp::Globals::Seat::get_pointer (wl_client *client, wl_resource *resource, UInt32 id)
{
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);

    Int32 version = wl_resource_get_version(resource);

    printf("Pointer Version: %i\n",version);
    wl_resource *pointer = wl_resource_create(client, &wl_pointer_interface, version, id); // 7
    wl_resource_set_implementation(pointer, &pointer_implementation, wClient,&Pointer::resource_destroy);
    wClient->setPointer(pointer);
    wClient->_wl_pointer_version = version;
}

void Wpp::Globals::Seat::get_keyboard (wl_client *client, wl_resource *resource, UInt32 id)
{
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);

    Int32 version = wl_resource_get_version(resource);
    wl_resource *keyboard = wl_resource_create(client, &wl_keyboard_interface,version,id); // 7
    wl_resource_set_implementation(keyboard, &keyboard_implementation, wClient, &Keyboard::resource_destroy);
    wClient->setKeyboard(keyboard);
    wl_keyboard_send_repeat_info(keyboard,32,512);

    printf("Keyboard Version: %i\n",version);

    //if(version >= 7)
        //wl_keyboard_send_keymap(keyboard, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1,WInput::getKeymapFD(true),WInput::getKeymapSize());
    //else
    wl_keyboard_send_keymap(keyboard, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1,WInput::getKeymapFD(false),WInput::getKeymapSize());

}

void Wpp::Globals::Seat::get_touch(wl_client *client, wl_resource *resource, UInt32 id)
{
    (void)client;(void)resource;(void)id;
}

void Wpp::Globals::Seat::release( wl_client *client, wl_resource *resource)
{
    (void)client;
    printf("SEAT RELEASED\n");
    Seat::resource_destroy(resource);
}

void Wpp::Globals::Seat::bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    (void)version;

    WCompositor *compositor = (WCompositor*)data;

    wl_resource *seat = wl_resource_create(client, &wl_seat_interface,version,id);
    WClient *wClient = *find_if(compositor->clients.begin(),compositor->clients.end(),[client](WClient *x) { return x->getClient() == client;});
    wl_resource_set_implementation(seat, &seat_implementation, wClient, &Seat::resource_destroy);
    wl_seat_send_capabilities(seat, WL_SEAT_CAPABILITY_POINTER | WL_SEAT_CAPABILITY_KEYBOARD);

}

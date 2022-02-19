#include "Seat.h"

#include <WCompositor.h>

#include <globals/Pointer.h>
#include <globals/Keyboard.h>
#include <WInput.h>

static struct wl_pointer_interface pointer_implementation = {&pointer_set_cursor, &pointer_release};
static struct wl_keyboard_interface keyboard_implementation = {&keyboard_release};
static struct wl_seat_interface seat_implementation = {&seat_get_pointer, &seat_get_keyboard, &seat_get_touch,&seat_release};

// SEAT
void seat_get_pointer (wl_client *client, wl_resource *resource, UInt32 id)
{
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    wl_resource *pointer = wl_resource_create (client, &wl_pointer_interface, 7, id);
    wl_resource_set_implementation (pointer, &pointer_implementation, wClient, NULL);
    wClient->setPointer(pointer);
}

void seat_get_keyboard (wl_client *client, wl_resource *resource, UInt32 id)
{
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    wl_resource *keyboard = wl_resource_create(client, &wl_keyboard_interface,7, id);
    wl_resource_set_implementation(keyboard, &keyboard_implementation, NULL, NULL);
    wClient->setKeyboard(keyboard);
    wl_keyboard_send_keymap(keyboard, WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1,getKeymapFD(),getKeymapSize());
}

void seat_get_touch (wl_client *client, wl_resource *resource, UInt32 id)
{
    (void)client;(void)resource;(void)id;
}

void seat_release( wl_client *client, wl_resource *resource)
{
    (void)client;
    printf("SEAT RELEASED\n");
    wl_resource_destroy(resource);
}

void seat_bind(wl_client *client, void *data, UInt32 version, UInt32 id)
{
    (void)version;

    WCompositor *compositor = (WCompositor*)data;

    wl_resource *seat = wl_resource_create(client, &wl_seat_interface,version,id);
    WClient *wClient = *find_if(compositor->clients.begin(),compositor->clients.end(),[client](WClient *x) { return x->getClient() == client;});
    wl_resource_set_implementation(seat, &seat_implementation, wClient, NULL);
    wl_seat_send_capabilities (seat, WL_SEAT_CAPABILITY_POINTER|WL_SEAT_CAPABILITY_KEYBOARD);

}

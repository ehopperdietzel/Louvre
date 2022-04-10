#include "Keyboard.h"
#include <WClient.h>
#include <stdio.h>

void WaylandPlus::Globals::Keyboard::resource_destroy(wl_resource *resource)
{
    printf("KEYBOARD RELEASED\n");
    WClient *client = (WClient*)wl_resource_get_user_data(resource);
    client->setKeyboard(nullptr);
}

void WaylandPlus::Globals::Keyboard::release(wl_client *client, wl_resource *resource)
{
    (void)client;
    Keyboard::resource_destroy(resource);
}

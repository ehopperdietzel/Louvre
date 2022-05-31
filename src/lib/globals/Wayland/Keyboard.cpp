#include "Keyboard.h"
#include <LClient.h>
#include <stdio.h>

void Louvre::Globals::Keyboard::resource_destroy(wl_resource *resource)
{
    printf("KEYBOARD RELEASED\n");
    LClient *client = (LClient*)wl_resource_get_user_data(resource);
    client->p_keyboardResource = nullptr;
}

void Louvre::Globals::Keyboard::release(wl_client *client, wl_resource *resource)
{
    (void)client;
    Keyboard::resource_destroy(resource);
}

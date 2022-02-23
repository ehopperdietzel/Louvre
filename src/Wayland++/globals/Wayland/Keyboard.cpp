#include "Keyboard.h"
#include <WClient.h>
#include <stdio.h>

void WaylandPlus::Globals::Keyboard::release(wl_client *client, wl_resource *resource)
{
    printf("KEYBOARD RELEASED\n");
    (void)client;
    WClient *wClient = (WClient*)wl_resource_get_user_data(resource);
    wClient->setKeyboard(nullptr);
}

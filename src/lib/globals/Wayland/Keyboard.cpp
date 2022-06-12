#include "Keyboard.h"
#include <LClient.h>
#include <stdio.h>

void Louvre::Globals::Keyboard::resource_destroy(wl_resource *resource)
{
    printf("KEYBOARD DESTROYED.\n");
    LClient *client = (LClient*)wl_resource_get_user_data(resource);
    client->p_keyboardResource = nullptr;
}

#if LOUVRE_SEAT_VERSION >= 3
void Louvre::Globals::Keyboard::release(wl_client *, wl_resource *resource)
{
    wl_resource_destroy(resource);
}
#endif

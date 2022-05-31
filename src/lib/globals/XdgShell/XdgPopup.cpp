#include "XdgPopup.h"

#include <LCompositor.h>
#include <LSurface.h>
#include <LClient.h>
#include <LPositioner.h>
#include <LPopup.h>

using namespace Louvre;

void Extensions::XdgShell::Popup::destroy_resource(wl_resource *resource)
{
    LPopupRole *wPopup = (LPopupRole*)wl_resource_get_user_data(resource);

    wPopup->surface()->compositor()->destroyPopupRequest(wPopup);

    if(wPopup->positioner())
        delete wPopup->p_positioner;

    delete wPopup;

    printf("POPUP DESTROYED.\n");
}

void Extensions::XdgShell::Popup::destroy(wl_client *client, wl_resource *resource)
{
    (void)client;
    wl_resource_destroy(resource);
}
void Extensions::XdgShell::Popup::grab(wl_client *client, wl_resource *resource, wl_resource *seat, UInt32 serial)
{
    (void)client;(void)resource;(void)seat;(void)serial;
    //LSurface *surface = (LSurface*)wl_resource_get_user_data(resource);
    //surface->grabSeatRequest();
    // TODO add serial
}
void Extensions::XdgShell::Popup::reposition(wl_client *client, wl_resource *resource, wl_resource *positioner, UInt32 serial)
{
    (void)client;

    LPositioner *wPositioner = (LPositioner*)wl_resource_get_user_data(positioner);
    LPopupRole *wPopup = (LPopupRole*)wl_resource_get_user_data(resource);
    wPopup->p_repositionSerial = serial;

    if(wPopup->positioner())
        delete wPopup->p_positioner;

    wPopup->p_positioner = wPositioner;
    wPopup->configureRequest();

}

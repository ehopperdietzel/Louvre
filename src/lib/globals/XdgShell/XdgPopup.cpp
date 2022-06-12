#include "XdgPopup.h"

#include <LCompositor.h>
#include <LSurface.h>
#include <LClient.h>
#include <LPositioner.h>
#include <LPopupRole.h>
#include <xdg-shell.h>

using namespace Louvre;

void Extensions::XdgShell::Popup::destroy_resource(wl_resource *resource)
{
    printf("XDG_POPUP DESTROYED.\n");
    LPopupRole *lPopup = (LPopupRole*)wl_resource_get_user_data(resource);

    // Unset role
    if(lPopup->surface())
    {
        lPopup->surface()->current.type = LSurface::Undefined;
        lPopup->surface()->p_role = nullptr;
        lPopup->surface()->typeChangeRequest();
    }

    // Notify
    lPopup->compositor()->destroyPopupRequest(lPopup);

    if(lPopup->positioner())
        delete lPopup->p_positioner;

    delete lPopup;
}

void Extensions::XdgShell::Popup::destroy(wl_client *, wl_resource *resource)
{
    LPopupRole *lPopup = (LPopupRole*)wl_resource_get_user_data(resource);

    if(!lPopup->surface()->children().empty())
    {
        /*
        wl_resource_post_error(
                    lPopup->surface()->client()->xdgWmBaseResource(),
                    XDG_WM_BASE_ERROR_NOT_THE_TOPMOST_POPUP,
                    "The client tried to map or destroy a non-topmost popup.");*/

        printf("IGNORING CLIENT ERROR: The client tried to map or destroy a non-topmost popup.\n");
    }

    wl_resource_destroy(resource);
}
void Extensions::XdgShell::Popup::grab(wl_client *, wl_resource *resource, wl_resource */*seat*/, UInt32 /*serial*/)
{
    LPopupRole *lPopup = (LPopupRole*)wl_resource_get_user_data(resource);

    /* Parent popup must have focus (Not really necesary)
    if(lPopup->surface()->parent()->roleType() == LSurface::Popup && )
    {

    }
    */

    lPopup->grabSeatRequest();
}

#if LOUVRE_XDG_WM_BASE_VERSION >= 3
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
#endif
